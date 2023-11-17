const axios = require('axios');


class SolverAiClientCompute {

    constructor(computerUrl, token, problemId) {
        this.__base_url_Computer = computerUrl + "/";
        this.__problemId = problemId;
        this.__headers = {
            Authorization: `Token ${token}`,
            "Content-Type": "application/json"
        };
    }

    static __isStatusCodeOk(statusCode) {
        return 200 <= statusCode && statusCode < 300;
    }

    async getProblemSetup() {
        try {
            const url = `${this.__base_url_Computer}problem_setup/${this.__problemId}`;
            const response = await axios.get(url, { headers: this.__headers });
            
            if (this.constructor.__isStatusCodeOk(response.status)) {
                return response.data;
            } else {
                throw new Error(`Failed with code: ${response.data}`);
            }
        } catch (error) {
            throw new Error(`Exception: ${error.message} - ${error.response.statusText}`);
        }
    }

    async runSolver(inputJson) {
        try {
            const url = `${this.__base_url_Computer}solvejson/`;
            const response = await axios.post(url, inputJson, { headers: this.__headers });
            
            if (this.constructor.__isStatusCodeOk(response.status)) {
                return response.data.results;
            } else {
                throw new Error(`Failed with code: ${response.data}`);
            }
        } catch (error) {
            throw new Error(`Exception: ${error.message}`);
        }
    }
}

module.exports = SolverAiClientCompute;
