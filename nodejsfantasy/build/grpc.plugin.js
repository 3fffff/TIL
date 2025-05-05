"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || (function () {
    var ownKeys = function(o) {
        ownKeys = Object.getOwnPropertyNames || function (o) {
            var ar = [];
            for (var k in o) if (Object.prototype.hasOwnProperty.call(o, k)) ar[ar.length] = k;
            return ar;
        };
        return ownKeys(o);
    };
    return function (mod) {
        if (mod && mod.__esModule) return mod;
        var result = {};
        if (mod != null) for (var k = ownKeys(mod), i = 0; i < k.length; i++) if (k[i] !== "default") __createBinding(result, mod, k[i]);
        __setModuleDefault(result, mod);
        return result;
    };
})();
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.grpcServerPlugin = void 0;
const fastify_plugin_1 = __importDefault(require("fastify-plugin"));
const path_1 = require("path");
const grpc = __importStar(require("@grpc/grpc-js"));
const protoLoader = __importStar(require("@grpc/proto-loader"));
const grpcServerOptions = {
    keepCase: false,
    longs: String,
    enums: String,
    defaults: false,
    oneofs: true,
};
const grpcServerPlugin = (0, fastify_plugin_1.default)(async (fastify) => {
    // load proto files from directory
    const packageDefinition = protoLoader.loadSync([(0, path_1.join)(__dirname, '../proto/example.proto')], grpcServerOptions);
    const proto = grpc.loadPackageDefinition(packageDefinition);
    const grpcServer = new grpc.Server();
    // mapping between handlers and rpc services
    grpcServer.addService(proto.example.UserService.service, {
        GetUsers: async (req, res) => {
            return res(null, {
                users: [{
                        id: 'test',
                        email: 'test',
                    }],
            });
        },
    });
    function start(opts = { port: 50501 }) {
        return grpcServer.bindAsync(`0.0.0.0:${opts.port}`, grpc.ServerCredentials.createInsecure(), (err, port) => {
            if (err) {
                console.error(err);
                return;
            }
            grpcServer.start();
            console.log(`GRPC Server listening on ${port}`);
        });
    }
    fastify.decorate('grpcServer', { start });
});
exports.grpcServerPlugin = grpcServerPlugin;
