import requests
import json

from SolverAiComputeInput import SolverAiComputeInput
from SolverAiComputeResults import SolverAiComputeResults


class SolverAiClientCompute:

    def __init__(self, computerUrl: str, token: str, problemId: int) -> None:
        self.__base_url_Computer = computerUrl + "/"
        self.__problemId = problemId
        self.__headers = {
            "Authorization": f"Token {token}"
        }

    @staticmethod
    def __isStatusCodeOk(response):
        statusCode = response.status_code
        return 200 <= statusCode and statusCode < 300

    def getProblemSetup(self):
        headers = self.__headers.copy()
        headers["Content-Type"] = "application/json"
        url = f'{self.__base_url_Computer}problem_setup/{self.__problemId}'
        response = requests.get(url, headers=headers)
        if self.__isStatusCodeOk(response):
            try:
                data = json.loads(response.text)
                return data['inputs'], data['outputs']
            except Exception:
                raise Exception('Failed retrieving data.')
        else:
            raise Exception(f'Failed with code: {json.loads(response.text)}.')

    def runSolver(self, input: SolverAiComputeInput) -> SolverAiComputeResults:
        headers = self.__headers.copy()
        headers["Content-Type"] = "application/json"
        url = f'{self.__base_url_Computer}solvejson/'
        jsonData = input.getJson()
        response = requests.post(url, headers=headers, data=jsonData)
        if self.__isStatusCodeOk(response):
            try:
                data = json.loads(response.text)
            except Exception:
                return Exception('Failed retrieving data.')
            return SolverAiComputeResults(data['results'])
        else:
            return Exception(f'Failed with code: {json.loads(response.text)}.')
