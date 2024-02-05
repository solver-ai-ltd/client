const fs = require('fs');
const createCsvWriter = require('csv-writer').createArrayCsvWriter;


class SolverAiResultsWriter {
    constructor(results) {
        this.results = results;
    }

    write(path) {
        const csvWriter = createCsvWriter({
            path: path,
            append: false
        });

        const numberOfResults = this.results.getNumberOfResults();
        const inputVariableNames = this.results.getInputVariableNames();
        const outputVariableNames = this.results.getOutputVariableNames();
        const X = this.results.getX();
        const Y = this.results.getY();

        // Write headers and initial data
        csvWriter.writeRecords([
            ['Number Of Results', numberOfResults],
            ['Objective Variable Names', ...this.results.getObjectiveVariableNames()],
            ['Constraint Variable Names', ...this.results.getConstraintVariableNames()],
            [''],
            ['Inputs'],
            ['#', ...inputVariableNames]
        ]).then(() => {
            // Write input data
            for (let i = 0; i < numberOfResults; i++) {
                csvWriter.writeRecords([['X' + i, ...X[i]]]);
            }
        }).then(() => {
            // Write output headers and data
            csvWriter.writeRecords([
                [''],
                ['Outputs'],
                ['#', ...outputVariableNames]
            ]).then(() => {
                for (let i = 0; i < numberOfResults; i++) {
                    csvWriter.writeRecords([['Y' + i, ...Y[i]]]);
                }
            });
        }).then(() => {
            console.log('...Done');
        });
    }
}

module.exports = SolverAiResultsWriter;
