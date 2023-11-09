const axios = require('axios');
const fs = require('fs');
const FormData = require('form-data');

class SolverAiClientSetup {
    constructor(datamanagerUrl, token) {
        this.__base_url_DM = datamanagerUrl + "/api/data/";
        this.__headers = {
            Authorization: `Token ${token}`
        };
        this.__equationSuffix = 'equations';
        this.__codeSuffix = 'code';
        this.__hardDataSuffix = 'hard-datas';
        this.__softDataSuffix = 'soft-datas';
        this.__problemSuffix = 'problems';
    }

    static __isStatusCodeOk(statusCode) {
        return 200 <= statusCode && statusCode < 300;
    }

    async _post(urlSuffix, data, formData = null) {
        try {
            const url = `${this.__base_url_DM}${urlSuffix}/`;
            let response;

            if ('vectorizationIndices' in data && !data['vectorizationIndices']) {
                delete data['vectorizationIndices'];
            }

            if (formData) {
                Object.keys(data).forEach(key => formData.append(key, data[key]));
                const formDataBuffer = formData.getBuffer();
                const headers = formData.getHeaders();
                headers['Content-Length'] = formDataBuffer.length;
                response = await axios.post(url, formDataBuffer, { headers: { ...this.__headers, ...headers } });
            } else {
                response = await axios.post(url, data, { headers: { ...this.__headers, "Content-Type": "application/json" } });
            }

            if (SolverAiClientSetup.__isStatusCodeOk(response.status)) {
                return response.data.id;
            } else {
                throw new Error(`Failed with code: ${JSON.stringify(response.data)}`);
            }
        } catch (error) {
            throw new Error(`Exception: ${error.message} - ${error.response.statusText}`);
        }
    }

    async _patchFile(urlSuffix, id, formData) {
        try {
            const url = `${this.__base_url_DM}${urlSuffix}/${id}/`;
            const formDataBuffer = formData.getBuffer();
            const headers = formData.getHeaders();
            headers['Content-Length'] = formDataBuffer.length;
            const response = await axios.patch(url, formDataBuffer, { headers: { ...this.__headers, ...headers } });
    
            if (SolverAiClientSetup.__isStatusCodeOk(response.status)) {
                return response.data.id;
            } else {
                throw new Error(`Failed with code: ${JSON.stringify(response.data)}`);
            }
        } catch (error) {
            throw new Error(`Exception: ${error.message} - ${error.response.statusText}`);
        }
    }

    async deleteAll(
        equationIds = [],
        codeIds = [],
        hardIds = [],
        softIds = [],
        problemId = null
    ) {
        var errors = ''
        try {
            if (problemId != null) {
                // Problem must be deleted first or will not allow deletion of models
                errors += await this.__deleteIds(this.__problemSuffix, [problemId]);
            }
            errors += await this.__deleteIds(this.__equationSuffix, equationIds);
            errors += await this.__deleteIds(this.__codeSuffix, codeIds);
            errors += await this.__deleteIds(this.__hardDataSuffix, hardIds);
            errors += await this.__deleteIds(this.__softDataSuffix, softIds);
        } catch (error) {
            console.error(`Failed Deleting: ${error.message}`);
        }
        if (errors.length !== 0) {
            throw new Error(errors)
        }
    }

    async __deleteIds(urlSuffix, ids) {
        var errors = '';
        for (const id of ids) {
            const url = `${this.__base_url_DM}${urlSuffix}/${id}`;
            try {
                await axios.delete(url, { headers: this.__headers });
            } catch (error) {
                errors += `Failed Deleting: ${error.message}\n`;
            }
        }
        return errors;
    }

    async postEquation(name, equationString, variablesString, vectorizationIndices = '') {
        const data = {
            name,
            equationString,
            variablesString,
            vectorizationIndices
        };
        return this._post(this.__equationSuffix, data);
    }

    async postCode(name, filePath, variablesStringIn, variablesStringOut, vectorizationIndices = '') {
        const formData = new FormData();
        formData.append('code', fs.readFileSync(filePath, 'binary'),
            {filename: 'code.py'}
        );
        const data = {
            name,
            variablesStringIn,
            variablesStringOut,
            vectorizationIndices
        };
        return this._post(this.__codeSuffix, data, formData);
    }

    async postHardData(name, filePath, vectorizationIndices = '') {
        const formData = new FormData();
        formData.append('csv', fs.readFileSync(filePath, 'binary'),
            {filename: 'data.csv'}
        );
        const data = {
            name,
            vectorizationIndices
        };
        return this._post(this.__hardDataSuffix, data, formData);
    }

    async postSoftData(name, filePath, variablesStringIn, variablesStringOut, vectorizationIndices = '') {
        const formData = new FormData();
        formData.append('csv', fs.readFileSync(filePath, 'binary'),
            {filename: 'data.csv'}
        );
        const data = {
            name,
            variablesStringIn,
            variablesStringOut,
            vectorizationIndices
        };
        return this._post(this.__softDataSuffix, data, formData);
    }

    async patchHardData(id, filePath) {
        const formData = new FormData();
        formData.append('csv', fs.readFileSync(filePath, 'binary'),
            {filename: 'data.csv'}
        );
        return this._patchFile(this.__hardDataSuffix, id, formData);
    }

    async patchSoftData(id, filePath) {
        const formData = new FormData();
        formData.append('csv', fs.readFileSync(filePath, 'binary'),
            {filename: 'data.csv'}
        );
        return this._patchFile(this.__softDataSuffix, id, formData);
    }

    async postProblem(
        problemName,
        equationIds = [],
        codeIds = [],
        hardIds = [],
        softIds = []
    ) {
        const data = {
            name: problemName,
            equations: equationIds,
            codes: codeIds,
            harddatas: hardIds,
            softdatas: softIds,
            tags: []
        };
        return this._post(this.__problemSuffix, data);
    }
}

module.exports = SolverAiClientSetup;
