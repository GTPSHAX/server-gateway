const { createServer } = require('https');
const http = require('http');
const { parse } = require('url');
const next = require('next');
const fs = require('fs');
const path = require('path');
const { default: consola } = require('consola');

// Check if -dev flag is passed
const isDev = process.argv.includes('--dev');
const dev = isDev;
const app = next({ dev });
const handle = app.getRequestHandler();

consola.log(`Running in ${dev ? 'development' : 'production'} mode`);

const httpsOptions = {
  key: fs.readFileSync(path.join(__dirname, 'cert', 'loginurl.growplus.asia-key.pem')),
  cert: fs.readFileSync(path.join(__dirname, 'cert', 'loginurl.growplus.asia-crt.pem')),
  ca: fs.readFileSync(path.join(__dirname, 'cert', 'loginurl.growplus.asia-chain-only.pem'))
};

app.prepare().then(() => {
  // Create HTTPS server
  createServer(httpsOptions, (req, res) => {
    const parsedUrl = parse(req.url, true);
    handle(req, res, parsedUrl);
  }).listen(3000, (err) => {
    if (err) throw err;
    console.log(`> Ready on https://localhost:3000 (${dev ? 'development' : 'production'} mode)`);
  });

  // Create HTTP server (port 80)
  http.createServer((req, res) => {
    const parsedUrl = parse(req.url, true);
    handle(req, res, parsedUrl);
  }).listen(80, (err) => {
    if (err) throw err;
    console.log(`> Ready on http://localhost:80 (${dev ? 'development' : 'production'} mode)`);
  });
});
