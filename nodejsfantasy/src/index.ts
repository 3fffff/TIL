import { buildApp, AppOptions } from './app';
import { main } from './services';

const options: AppOptions = {
  logger: true,
};

const start = async () => {
  const app = await buildApp(options);

  try {
    await app.listen({
      port: 3000,
      host: 'localhost',
    });

    app.grpcServer.start();
    await main(app.prisma);
  } catch (err) {
    app.log.error(err);
    process.exit(1);
  }
};

start();