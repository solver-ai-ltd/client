from enum import Enum
from json import dumps


class CONSTRAINT(Enum):
    SMALLER_THAN = 'smaller than'
    GREATER_THAN = 'greater than'
    EQUAL_TO = 'equal to'
    INSIDE_RANGE = 'inside range'
    OUTSIDE_RANGE = 'outside range'


class OBJECTIVE(Enum):
    MINIMIZE = 'minimize'
    MAXIMIZE = 'maximize'


class SolverAiComputeInput:
    def __init__(self, problem_id):
        self.problem_id = problem_id
        self.inputs = {}
        self.constraints = {}
        self.objectives = {}

    def add_input(self, name, Min, Max, is_constant, is_integer):
        self.inputs[name] = {
            'Min': Min,
            'Max': Max,
            'Constant': is_constant,
            'Integer': is_integer
        }

    def add_constraint(self, name, operation, value1, value2=0):
        self.constraints[name] = {
            'Operation': operation.value,
            'Value1': value1,
            'Value2': value2
        }

    def add_objective(self, name, operation):
        self.objectives[name] = {
            'Operation': operation.value}

    def get_json(self):
        return dumps({
            'id': self.problem_id,
            'inputs': self.inputs,
            'constraints': self.constraints,
            'objectives': self.objectives
        })
