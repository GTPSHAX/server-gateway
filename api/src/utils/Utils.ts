import path from 'path';
import fs from 'fs';
import type { FastifyInstance } from 'fastify';

export const LoadRoutes = async (app: FastifyInstance, routesDir: string = './routes') => {
  const absolutePath = path.resolve(routesDir);
  
  if (!fs.existsSync(absolutePath)) {
    console.warn(`Routes directory ${absolutePath} does not exist`);
    return;
  }

  const files = fs.readdirSync(absolutePath);
  const routePromises: Promise<void>[] = [];
  
  files.forEach(file => {
    // Only process .ts files (excluding .d.ts declaration files)
    if (file.endsWith('.ts') && !file.endsWith('.d.ts')) {
      const fileName = file.replace('.ts', '');
      
      // Skip index files
      if (fileName === 'index') return;
      
      try {
        const modulePath = path.join(absolutePath, file);
        
        // Create promise for each route registration
        const routePromise = import(modulePath)
          .then((routeModule) => {
            const routeHandler = routeModule.default || routeModule;
            app.register(routeHandler, { prefix: `/${fileName}` });
            console.log(`Loaded route: /${fileName}`);
          })
          .catch((error) => {
            console.error(`Failed to load route ${fileName}:`, error);
          });
        
        routePromises.push(routePromise);
      } catch (error) {
        console.error(`Failed to load route ${fileName}:`, error);
      }
    }
  });

  // Wait for all routes to be loaded
  await Promise.all(routePromises);
};