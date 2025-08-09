import fastify from 'fastify';
import { LoadRoutes } from './utils/Utils';

const app = fastify();
const APP_PORT = process.env.APP_PORT || 8080;

const start = async () => {
  try {
    await LoadRoutes(app, './src/routes');
    await app.listen({ port: APP_PORT as number });
    console.log('Server running on http://localhost:' + APP_PORT);
  } catch (err) {
    app.log.error(err);
    process.exit(1);
  }
};

start();