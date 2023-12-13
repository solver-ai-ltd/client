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

    async _postPatch(urlSuffix, data, formData, id = null) {
        let isPost = true;
        if (id) {
            isPost = false;
        }
        try {
            let url = '';
            if (isPost) {
                url = `${this.__base_url_DM}${urlSuffix}/`;
            } else {
                url = `${this.__base_url_DM}${urlSuffix}/${id}/`;
            }
            let response;

            if ('vectorizationIndices' in data && !data['vectorizationIndices']) {
                delete data['vectorizationIndices'];
            }

            if (formData) {
                Object.keys(data).forEach(key => formData.append(key, data[key]));
                const formDataBuffer = formData.getBuffer();
                const headers = formData.getHeaders();
                headers['Content-Length'] = formDataBuffer.length;
                if (isPost) {
                    response = await axios.post(url, formDataBuffer, { headers: { ...this.__headers, ...headers } });
                } else {
                    response = await axios.patch(url, formDataBuffer, { headers: { ...this.__headers, ...headers } });
                }
            } else {
                if (isPost) {
                    response = await axios.post(url, data, { headers: { ...this.__headers, "Content-Type": "application/json" } });
                } else {
                    response = await axios.patch(url, data, { headers: { ...this.__headers, "Content-Type": "application/json" } });
                }
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

    async __deleteId(urlSuffix, id) {
        var error = '';
        const url = `${this.__base_url_DM}${urlSuffix}/${id}`;
        try {
            await axios.delete(url, { headers: this.__headers });
        } catch (error) {
            error = `Failed Deleting: ${error.message}\n`;
        }
        return error;
    }

    async __deleteIds(urlSuffix, ids) {
        var errors = '';
        for (const id of ids) {
            errors += await this.__deleteId(urlSuffix, id);
        }
        return errors;
    }

    async deleteEquation(id) {
        return self.__deleteId(this.__equationSuffix, id);
    }

    async deleteCode(id) {
        return self.__deleteId(this.__codeSuffix, id);
    }

    async deleteHardData(id) {
        return self.__deleteId(this.__hardDataSuffix, id);
    }

    async deleteSoftData(id) {
        return self.__deleteId(this.__softDataSuffix, id);
    }

    async deleteProblem(id) {
        return self.__deleteId(this.__problemSuffix, id);
    }

    async postEquation(
        name,
        equationString,
        variablesString,
        vectorizationIndices = ''
    ) {
        const data = {
            name,
            equationString,
            variablesString,
            vectorizationIndices
        };
        return this._postPatch(this.__equationSuffix, data, null);
    }

    async patchEquation(
        id, name = '',
        equationString = '',
        variablesString = '',
        vectorizationIndices = ''
    ) {
        const data = {};
        if (name !== '') data.name = name;
        if (equationString !== '') data.equationString = equationString;
        if (variablesString !== '') data.variablesString = variablesString;
        if (vectorizationIndices !== '') data.vectorizationIndices = vectorizationIndices;
        return this._postPatch(this.__equationSuffix, data, null, id);
    }

    async postCode(
        name,
        filePath,
        variablesStringIn,
        variablesStringOut,
        vectorizationIndices = ''
    ) {
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
        return this._postPatch(this.__codeSuffix, data, formData);
    }

    async patchCode(
        id,
        name = '',
        filePath = '',
        variablesStringIn = '',
        variablesStringOut = '',
        vectorizationIndices = ''
    ) {
        let formData = null;
        if (filePath !== '') {
            formData = new FormData();
            formData.append('code', fs.readFileSync(filePath, 'binary'),
                {filename: 'code.py'}
            );
        }
        const data = {};
        if (name !== '') data.name = name;
        if (variablesStringIn !== '') data.variablesStringIn = variablesStringIn;
        if (variablesStringOut !== '') data.variablesStringOut = variablesStringOut;
        if (vectorizationIndices !== '') data.vectorizationIndices = vectorizationIndices;
        return this._postPatch(this.__codeSuffix, data, formData, id);
    }

    async postHardData(
        name,
        filePath,
        vectorizationIndices = ''
    ) {
        const formData = new FormData();
        formData.append('csv', fs.readFileSync(filePath, 'binary'),
            {filename: 'data.csv'}
        );
        const data = {
            name,
            vectorizationIndices
        };
        return this._postPatch(this.__hardDataSuffix, data, formData);
    }

    async patchHardData(
        id,
        name = '',
        filePath = '',
        vectorizationIndices = ''
    ) {
        let formData = null;
        if (filePath !== '') {
            formData = new FormData();
            formData.append('csv', fs.readFileSync(filePath, 'binary'),
                {filename: 'data.csv'}
            );
        }
        const data = {};
        if (name !== '') data.name = name;
        if (vectorizationIndices !== '') data.vectorizationIndices = vectorizationIndices;
        return this._postPatch(this.__hardDataSuffix, data, formData, id);
    }

    async postSoftData(
        name,
        filePath,
        variablesStringIn,
        variablesStringOut,
        vectorizationIndices = ''
    ) {
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
        return this._postPatch(this.__softDataSuffix, data, formData);
    }

    async patchSoftData(
        id, name = '',
        filePath = '',
        variablesStringIn = '',
        variablesStringOut = '',
        vectorizationIndices = ''
    ) {
        let formData = null;
        if (filePath !== '') {
            formData = new FormData();
            formData.append('csv', fs.readFileSync(filePath, 'binary'),
                {filename: 'data.csv'}
            );
        }
        const data = {};
        if (name !== '') data.name = name;
        if (variablesStringIn !== '') data.variablesStringIn = variablesStringIn;
        if (variablesStringOut !== '') data.variablesStringOut = variablesStringOut;
        if (vectorizationIndices !== '') data.vectorizationIndices = vectorizationIndices;
        return this._postPatch(this.__softDataSuffix, data, formData, id);
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
            softdatas: softIds
        };
        return this._postPatch(this.__problemSuffix, data, null);
    }

    async patchProblem(
        id,
        problemName = '',
        equationIds = [],
        codeIds = [],
        hardIds = [],
        softIds = []
    ) {
        const data = {};
        if (problemName !== '') data.name = problemName;
        if (equationIds.length > 0) data.equations = equationIds;
        if (codeIds.length > 0) data.codes = codeIds;
        if (hardIds.length > 0) data.harddatas = hardIds;
        if (softIds.length > 0) data.softdatas = softIds;

        return this._postPatch(this.__problemSuffix, data, null, id);
    }
}

module.exports = SolverAiClientSetup;
