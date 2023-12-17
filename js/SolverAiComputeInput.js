const CONSTRAINT = {
    SMALLER_THAN: 'smaller than',
    GREATER_THAN: 'greater than',
    EQUAL_TO: 'equal to',
    INSIDE_RANGE: 'inside range',
    OUTSIDE_RANGE: 'outside range'
};

const OBJECTIVE = {
    MINIMIZE: 'minimize',
    MAXIMIZE: 'maximize'
};

class SolverAiComputeInput {

    constructor(problem_id) {
        this.problem_id = problem_id;
        this.inputs = {};
        this.constraints = {};
        this.objectives = {};

        this.setSolverSetup();
    }

    setSolverSetup(includeLeastInfeasible=false, solutionQuality=1) {
        this.solverSetup = {
            'includeLeastInfeasible': Number(includeLeastInfeasible),
            'solutionQuality': solutionQuality
        }
    }

    addInput(name, Min, Max, is_constant, is_integer) {
        this.inputs[name] = {
            Min: Min,
            Max: Max,
            Constant: is_constant,
            Integer: is_integer
        };
    }

    addConstraint(name, operation, value1, value2=0) {
        if (!Object.values(CONSTRAINT).includes(operation)) {
            throw new Error(`Invalid constraint operation: ${operation}`);
        }
        this.constraints[name] = {
            Operation: operation,
            Value1: value1,
            Value2: value2
        };
    }

    addObjective(name, operation) {
        if (!Object.values(OBJECTIVE).includes(operation)) {
            throw new Error(`Invalid objective operation: ${operation}`);
        }
        this.objectives[name] = {
            Operation: operation
        };
    }

    getJson() {
        return {
            id: this.problem_id,
            solverSetup: this.solverSetup,
            inputs: this.inputs,
            constraints: this.constraints,
            objectives: this.objectives
        };
    }
}

module.exports = {
    SolverAiComputeInput: SolverAiComputeInput,
    CONSTRAINT: CONSTRAINT,
    OBJECTIVE: OBJECTIVE
};
