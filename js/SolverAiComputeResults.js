class SolverAiComputeResults {
    constructor(j) {
        this.numberOfResults = j["Number Of Results"];
        this.objectiveVariableNames = JSON.parse(j["Objective Variable Names"].replace(/'/g, '"'));
        this.constraintVariableNames = JSON.parse(j["Constraint Variable Names"].replace(/'/g, '"'));
        this.inputVariableNames = JSON.parse(j["Input Variable Names"].replace(/'/g, '"'));
        this.outputVariableNames = JSON.parse(j["Output Variable Names"].replace(/'/g, '"'));
        this.X = Array.from({length: this.numberOfResults}, (_, i) => JSON.parse(j["X" + i].replace(/'/g, '"')));
        this.Y = Array.from({length: this.numberOfResults}, (_, i) => JSON.parse(j["Y" + i].replace(/'/g, '"')));
    }

    getNumberOfResults() {
        return this.numberOfResults;
    }

    getObjectiveVariableNames() {
        return this.objectiveVariableNames;
    }

    getConstraintVariableNames() {
        return this.constraintVariableNames;
    }

    getInputVariableNames() {
        return this.inputVariableNames;
    }

    getOutputVariableNames() {
        return this.outputVariableNames;
    }

    getX() {
        return this.X;
    }

    getY() {
        return this.Y;
    }
}

module.exports = SolverAiComputeResults;
