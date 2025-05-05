"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const app_1 = require("./app");
const services_1 = require("./services");
const options = {
    logger: true,
};
const start = async () => {
    const app = await (0, app_1.buildApp)(options);
    try {
        await app.listen({
            port: 3000,
            host: 'localhost',
        });
        app.grpcServer.start();
        await (0, services_1.main)(app.prisma);
    }
    catch (err) {
        app.log.error(err);
        process.exit(1);
    }
};
start();
