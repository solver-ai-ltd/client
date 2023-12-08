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

    def _postPatch(self, urlSuffix, data: dict, file: tuple, id=None):
        isPost = True
        if id is not None:
            isPost = False

        if isPost:
            url = f'{self.__base_url_DM}{urlSuffix}/'
            httpFunction = requests.post
        else:
            url = f'{self.__base_url_DM}{urlSuffix}/{id}/'
            httpFunction = requests.patch

        tempData = data.copy()
        if 'vectorizationIndices' in tempData and \
                not tempData['vectorizationIndices']:
            tempData.pop('vectorizationIndices')
        if file is not None:
            with open(file[0], 'rb') as fp:
                response = httpFunction(
                    url, headers=self.__headers, data=tempData,
                    files={file[1]: fp}
                )
        else:  # If no files, convert data to a JSON string
            headers = self.__headers.copy()
            headers["Content-Type"] = "application/json"
            jsonData = json.dumps(tempData)
            response = httpFunction(
                url, headers=headers, data=jsonData
            )

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
        return self._postPatch(self.__equationSuffix, data, None)

    def patchEquation(
        self,
        id: int,
        name: str = '',
        equationString: str = '',
        variablesString: str = '',
        vectorizationIndices: str = ''
    ):
        data = dict()
        if name:
            data['name'] = name
        if equationString:
            data['equationString'] = equationString
        if variablesString:
            data['variablesString'] = variablesString
        if vectorizationIndices:
            data['vectorizationIndices'] = vectorizationIndices
        return self._postPatch(self.__equationSuffix, data, None, id)

    def postCode(
        self,
        name: str,
        filePath: str,
        variablesStringIn: str,
        variablesStringOut: str,
        vectorizationIndices: str = ''
    ):
        file = (filePath, 'code')
        data = {
            "name": name,
            "variablesStringIn": variablesStringIn,
            "variablesStringOut": variablesStringOut,
            "vectorizationIndices": vectorizationIndices
        }
        return self._postPatch(self.__codeSuffix, data, file)

    def patchCode(
        self,
        id: int,
        name: str = '',
        filePath: str = '',
        variablesStringIn: str = '',
        variablesStringOut: str = '',
        vectorizationIndices: str = ''
    ):
        file = None
        if filePath:
            file = (filePath, 'code')
        data = dict()
        if name:
            data['name'] = name
        if variablesStringIn:
            data['variablesStringIn'] = variablesStringIn
        if variablesStringOut:
            data['variablesStringOut'] = variablesStringOut
        if vectorizationIndices:
            data['vectorizationIndices'] = vectorizationIndices
        return self._postPatch(self.__codeSuffix, data, file, id)

    def postHardData(
        self,
        name: str,
        filePath: str,
        vectorizationIndices: str = ''
    ):
        file = (filePath, 'csv')
        data = {
            "name": name,
            "vectorizationIndices": vectorizationIndices
        }
        return self._postPatch(self.__hardDataSuffix, data, file)

    def patchHardData(
        self,
        id: int,
        name: str = '',
        filePath: str = '',
        vectorizationIndices: str = ''
    ):
        file = None
        if filePath:
            file = (filePath, 'csv')
        data = dict()
        if name:
            data['name'] = name
        if vectorizationIndices:
            data['vectorizationIndices'] = vectorizationIndices
        return self._postPatch(self.__hardDataSuffix, data, file, id)

    def postSoftData(
        self,
        name: str,
        filePath: str,
        variablesStringIn: str,
        variablesStringOut: str,
        vectorizationIndices: str = ''
    ):
        file = (filePath, 'csv')
        data = {
            "name": name,
            "variablesStringIn": variablesStringIn,
            "variablesStringOut": variablesStringOut,
            "vectorizationIndices": vectorizationIndices
        }
        return self._postPatch(self.__softDataSuffix, data, file)

    def patchSoftData(
        self,
        id: int,
        name: str = '',
        filePath: str = '',
        variablesStringIn: str = '',
        variablesStringOut: str = '',
        vectorizationIndices: str = ''
    ):
        file = None
        if filePath:
            file = (filePath, 'csv')
        data = dict()
        if name:
            data['name'] = name
        if variablesStringIn:
            data['variablesStringIn'] = variablesStringIn
        if variablesStringOut:
            data['variablesStringOut'] = variablesStringOut
        if vectorizationIndices:
            data['vectorizationIndices'] = vectorizationIndices
        return self._postPatch(self.__softDataSuffix, data, file, id)

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
        return self._postPatch(self.__problemSuffix, data, None)

    def patchProblem(
        self,
        id: int,
        name: str = '',
        equationIds=list(),
        codeIds=list(),
        hardIds=list(),
        softIds=list()
    ):
        data = dict()
        if name:
            data['name'] = name
        if len(equationIds):
            data['equations'] = equationIds
        if len(codeIds):
            data['codes'] = codeIds
        if len(hardIds):
            data['harddatas'] = hardIds
        if len(softIds):
            data['softdatas'] = softIds
        return self._postPatch(self.__problemSuffix, data, None, id)
