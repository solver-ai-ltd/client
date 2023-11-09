import requests
import json


class SolverAiClientSetup:

    class ID:
        def __init__(self, urlSuffix: str, id: int) -> None:
            self.urlSuffix = urlSuffix
            self.id = id

    def __init__(self, datamanagerUrl: str, token: str) -> None:
        self.__base_url_DM = datamanagerUrl + "/api/data/"
        self.__headers = {
            "Authorization": f"Token {token}"
        }
        self.__equationSuffix = "equations"
        self.__codeSuffix = "code"
        self.__hardDataSuffix = "hard-datas"
        self.__softDataSuffix = "soft-datas"
        self.__problemSuffix = "problems"

    def _post(self, urlSuffix, data: dict, files: dict = None):
        url = f'{self.__base_url_DM}{urlSuffix}/'
        tempData = data.copy()
        if 'vectorizationIndices' in tempData and \
                not tempData['vectorizationIndices']:
            tempData.pop('vectorizationIndices')
        if files is not None:
            response = requests.post(
                url, headers=self.__headers, data=tempData, files=files)
        else:  # If no files, convert data to a JSON string
            headers = self.__headers.copy()
            headers["Content-Type"] = "application/json"
            jsonData = json.dumps(tempData)
            response = requests.post(
                url, headers=headers, data=jsonData)
        return self.__processResponse(urlSuffix, response)

    def _patchFile(self, urlSuffix, id: int, files: dict = None):
        url = f'{self.__base_url_DM}{urlSuffix}/{id}/'
        response = requests.patch(
            url, headers=self.__headers, files=files)
        return self.__processResponse(urlSuffix, response)

    @staticmethod
    def __isStatusCodeOk(response):
        statusCode = response.status_code
        return 200 <= statusCode and statusCode < 300

    def __processResponse(self, urlSuffix, response):
        if self.__isStatusCodeOk(response):
            try:
                data = json.loads(response.text)
            except Exception:
                raise Exception('Failed retrieving data.')
            return data['id']
        else:
            raise Exception(f'Failed with code: {json.loads(response.text)}.')

    def deleteAll(
        self,
        equationIds=list(),
        codeIds=list(),
        hardIds=list(),
        softIds=list(),
        problemId=None
    ):
        errors = ''
        try:
            if problemId is not None:
                # Problem must be deleted first or will not allow deletion
                # of models
                errors += self.__deleteIds(self.__problemSuffix, [problemId])
            errors += self.__deleteIds(self.__equationSuffix, equationIds)
            errors += self.__deleteIds(self.__codeSuffix, codeIds)
            errors += self.__deleteIds(self.__hardDataSuffix, hardIds)
            errors += self.__deleteIds(self.__softDataSuffix, softIds)
        except Exception:
            raise Exception('Failed Deleting')
        if len(errors):
            raise Exception(errors)

    def __deleteIds(self, urlSuffix: str, ids: list) -> None:
        errors = ''
        for id in ids:
            url = f'{self.__base_url_DM}{urlSuffix}/{id}'
            response = requests.delete(url, headers=self.__headers)
            if not self.__isStatusCodeOk(response):
                errors = f'Failed Deleting: {url}\n'
        return errors

    def postEquation(
        self,
        name: str,
        equationString: str,
        variablesString: str,
        vectorizationIndices: str = ''
    ):
        data = {
            "name": name,
            "equationString": equationString,
            "variablesString": variablesString,
            "vectorizationIndices": vectorizationIndices
        }
        return self._post(self.__equationSuffix, data)

    def postCode(
        self,
        name: str,
        filePath: str,
        variablesStringIn: str,
        variablesStringOut: str,
        vectorizationIndices: str = ''
    ):
        with open(filePath, 'rb') as fp:
            files = {'code': fp}
            data = {
                "name": name,
                "variablesStringIn": variablesStringIn,
                "variablesStringOut": variablesStringOut,
                "vectorizationIndices": vectorizationIndices
            }
            return self._post(self.__codeSuffix, data, files)
        return None

    def postHardData(
        self,
        name: str,
        filePath: str,
        vectorizationIndices: str = ''
    ):
        with open(filePath, 'rb') as fp:
            files = {'csv': fp}
            data = {
                "name": name,
                "vectorizationIndices": vectorizationIndices
            }
            return self._post(self.__hardDataSuffix, data, files)
        return None

    def postSoftData(
        self,
        name: str,
        filePath: str,
        variablesStringIn: str,
        variablesStringOut: str,
        vectorizationIndices: str = ''
    ):
        with open(filePath, 'rb') as fp:
            files = {'csv': fp}
            data = {
                "name": name,
                "variablesStringIn": variablesStringIn,
                "variablesStringOut": variablesStringOut,
                "vectorizationIndices": vectorizationIndices
            }
            return self._post(self.__softDataSuffix, data, files)
        return None

    def patchHardData(
        self,
        id: int,
        filePath: str
    ):
        with open(filePath, 'rb') as fp:
            files = {'csv': fp}
            return self._patchFile(self.__hardDataSuffix, id, files)
        return None

    def patchSoftData(
        self,
        id: int,
        filePath: str
    ):
        with open(filePath, 'rb') as fp:
            files = {'csv': fp}
            return self._patchFile(self.__softDataSuffix, id, files)
        return None

    def postProblem(
        self,
        name: str,
        equationIds=list(),
        codeIds=list(),
        hardIds=list(),
        softIds=list()
    ):
        data = {
            "name": name,
            "equations": equationIds,
            "codes": codeIds,
            "harddatas": hardIds,
            "softdatas": softIds,
            "tags": []
        }
        return self._post(self.__problemSuffix, data)
