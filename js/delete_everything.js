const SolverAiClientSetup = require('./SolverAiClientSetup');

const config = require('./setup.js');


const solverAiClientSetup = 
    new SolverAiClientSetup(config.datamanagerUrl, config.token);

    async function main() {
        var exitCode = 1;

        try {
        const problemErrors = await solverAiClientSetup.deleteProblems();

        const otherErrors = await Promise.all([
            solverAiClientSetup.deleteEquations(),
            solverAiClientSetup.deleteCodes(),
            solverAiClientSetup.deleteSoftDatas(),
            solverAiClientSetup.deleteHardDatas()
        ]);

        const errors = problemErrors + otherErrors.join('');
        if (errors.length > 1) {
            throw new Error(errors);
        }

        exitCode = 0;
    } catch (error) {
        console.error(`Exception: ${error.message}`);
    }

    return exitCode;
}

(async () => {
    const exitCode = await main();
    process.exitCode = exitCode;
})();
