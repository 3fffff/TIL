"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const client_1 = require("@prisma/client");
const fastify_plugin_1 = __importDefault(require("fastify-plugin"));
async function initDatabaseConnection() {
    const db = new client_1.PrismaClient();
    await db.$connect();
    return db;
}
const prismaPlugin = (0, fastify_plugin_1.default)(async (server) => {
    const prisma = await initDatabaseConnection();
    // Make Prisma Client available through the fastify server instance: server.prisma
    server.decorate('prisma', prisma);
    server.addHook('onClose', async () => {
        await server.prisma.$disconnect();
    });
});
exports.default = prismaPlugin;
