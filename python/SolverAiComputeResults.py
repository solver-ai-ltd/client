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

    def get_number_of_results(self):
        return self.numberOfResults

    def get_objective_variable_names(self):
        return self.objectiveVariableNames

    def get_constraint_variable_names(self):
        return self.constraintVariableNames

    def get_input_variable_names(self):
        return self.inputVariableNames

    def get_output_variable_names(self):
        return self.outputVariableNames

    def get_X(self):
        return self.X

    def get_Y(self):
        return self.Y
