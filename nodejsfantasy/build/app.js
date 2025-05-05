"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.buildApp = buildApp;
const fastify_1 = __importDefault(require("fastify"));
const prisma_plugin_1 = __importDefault(require("./prisma.plugin"));
const grpc_plugin_1 = require("./grpc.plugin");
async function buildApp(options = {}) {
    const fastify = (0, fastify_1.default)(options);
    fastify.register(prisma_plugin_1.default);
    fastify.register(grpc_plugin_1.grpcServerPlugin);
    return fastify;
}
