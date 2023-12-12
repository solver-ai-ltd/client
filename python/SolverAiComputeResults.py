from ast import literal_eval


class SolverAiComputeResults:
    def __init__(self, j: dict):
        self.numberOfResults = j["Number Of Results"]
        self.objectiveVariableNames = \
            literal_eval(j["Objective Variable Names"])
        self.constraintVariableNames = \
            literal_eval(j["Constraint Variable Names"])
        self.inputVariableNames = literal_eval(j["Input Variable Names"])
        self.outputVariableNames = literal_eval(j["Output Variable Names"])
        self.X = [literal_eval(j["X" + str(i)])
                  for i in range(self.numberOfResults)]
        self.Y = [literal_eval(j["Y" + str(i)])
                  for i in range(self.numberOfResults)]

    def getNumberOfResults(self):
        return self.numberOfResults

    def getObjectiveVariableNames(self):
        return self.objectiveVariableNames

    def getConstraintVariableNames(self):
        return self.constraintVariableNames

    def getInputVariableNames(self):
        return self.inputVariableNames

    def getOutputVariableNames(self):
        return self.outputVariableNames

    def getX(self):
        return self.X

    def getY(self):
        return self.Y
