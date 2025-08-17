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

// Fungsi untuk memuat certificate berdasarkan hostname
function loadCertificate(hostname) {
  try {
    const certPath = path.join(__dirname, 'cert', hostname);
    
    // Cek apakah certifikat untuk hostname ini ada
    if (fs.existsSync(certPath)) {
      return {
        key: fs.readFileSync(path.join(certPath, `${hostname}-key.pem`)),
        cert: fs.readFileSync(path.join(certPath, `${hostname}-crt.pem`)),
        ca: fs.readFileSync(path.join(certPath, `${hostname}-chain-only.pem`))
      };
    }
    
    // Return default certificate jika tidak ada
    return {
      key: fs.readFileSync(path.join(__dirname, 'cert', 'loginurl.growplus.asia-key.pem')),
      cert: fs.readFileSync(path.join(__dirname, 'cert', 'loginurl.growplus.asia-crt.pem')),
      ca: fs.readFileSync(path.join(__dirname, 'cert', 'loginurl.growplus.asia-chain-only.pem'))
    };
  } catch (error) {
    consola.error(`Failed to load certificate for ${hostname}:`, error.message);
    // Return default certificate
    return {
      key: fs.readFileSync(path.join(__dirname, 'cert', 'loginurl.growplus.asia-key.pem')),
      cert: fs.readFileSync(path.join(__dirname, 'cert', 'loginurl.growplus.asia-crt.pem')),
      ca: fs.readFileSync(path.join(__dirname, 'cert', 'loginurl.growplus.asia-chain-only.pem'))
    };
  }
}

// Default certificate
const defaultCert = loadCertificate('loginurl.growplus.asia');

// SNI callback untuk memilih certificate berdasarkan hostname
const sniCallback = (servername, callback) => {
  consola.log(`SNI request for: ${servername}`);
  
  try {
    const cert = loadCertificate(servername);
    callback(null, cert);
  } catch (error) {
    consola.error(`SNI callback error for ${servername}:`, error.message);
    // Gunakan default certificate jika ada error
    callback(null, defaultCert);
  }
};

const httpsOptions = {
  ...defaultCert,
  SNICallback: sniCallback
};

app.prepare().then(() => {
  // Create HTTPS server
  createServer(httpsOptions, (req, res) => {
    const parsedUrl = parse(req.url, true);
    const hostname = req.headers.host?.split(':')[0] || 'localhost';
    consola.log(`HTTPS Request: ${req.method} ${req.url} from ${hostname}`);
    handle(req, res, parsedUrl);
  }).listen(3000, (err) => {
    if (err) throw err;
    console.log(`> Ready on https://localhost:3000 (${dev ? 'development' : 'production'} mode)`);
    console.log(`> SNI enabled - supporting multiple SSL certificates`);
  });

  // Create HTTP server (port 80)
  http.createServer((req, res) => {
    const parsedUrl = parse(req.url, true);
    const hostname = req.headers.host?.split(':')[0] || 'localhost';
    consola.log(`HTTP Request: ${req.method} ${req.url} from ${hostname}`);
    handle(req, res, parsedUrl);
  }).listen(80, (err) => {
    if (err) throw err;
    console.log(`> Ready on http://localhost:80 (${dev ? 'development' : 'production'} mode)`);
  });
});