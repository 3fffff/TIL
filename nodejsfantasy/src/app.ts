import Fastify, { FastifyServerOptions } from 'fastify'
import prismaPlugin from './prisma.plugin';
import { grpcServerPlugin } from './grpc.plugin';

export type AppOptions = Partial<FastifyServerOptions>;

async function buildApp(options: AppOptions = {}) {
  const fastify = Fastify(options);
  fastify.register(prismaPlugin);
  fastify.register(grpcServerPlugin);

  return fastify;
}

export { buildApp }