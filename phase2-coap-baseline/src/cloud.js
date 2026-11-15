'use strict';

const coap = require('coap');
const COAP_PORT = process.env.TRIAL_COAP_PORT === undefined
  ? 56830 : Number(process.env.TRIAL_COAP_PORT);
const BIND_ADDRESS = '0.0.0.0';
const RESOURCE_PATH = '/coap-test';
const PING_PAYLOAD = Buffer.from('PING', 'ascii');
const PONG_PAYLOAD = Buffer.from('PONG', 'ascii');
const SERVER_OPTIONS = {
  type: 'udp4',
  reuseAddr: false,
  proxy: false,
  piggybackReplyMs: 50,
  sendAcksForNonConfirmablePackets: true,
  cacheSize: 65536
};
let server_failed = false;

if (!Number.isInteger(COAP_PORT) || COAP_PORT < 1 || COAP_PORT > 65535) {
  throw new RangeError('TRIAL_COAP_PORT must be an integer from 1 to 65535');
}

const server = coap.createServer(SERVER_OPTIONS);

function onRequest(req, res) {
  res.on('error', onResponseError);
  console.log(`Request peer=${req.rsinfo.address}:${req.rsinfo.port} method=${req.method} path=${JSON.stringify(req.url)} bytes=${req.payload.length}`);
  res.setOption('Content-Format', 'text/plain');

  if (req.url !== RESOURCE_PATH) {
    res.code = '4.04';
    res.end();
    return;
  }
  if (req.method !== 'POST') {
    res.code = '4.05';
    res.end();
    return;
  }
  // Some option values remain Buffers and are absent from normalized headers.
  for (const option of req.options || []) {
    if (['Observe', 'Block1', 'Block2', 'Q-Block1', 'Q-Block2',
      'Proxy-Uri', 'Proxy-Scheme', 'OSCORE'].includes(option.name)) {
      res.code = '4.00';
      res.end();
      return;
    }
  }
  if (req.headers['Content-Format'] !== 'text/plain' ||
      req.headers.Observe !== undefined ||
      req.headers.Block1 !== undefined || req.headers.Block2 !== undefined ||
      req.headers['Proxy-Uri'] !== undefined || req.headers['Proxy-Scheme'] !== undefined ||
      req.headers.OSCORE !== undefined || !req.payload.equals(PING_PAYLOAD)) {
    res.code = '4.00';
    res.end();
    return;
  }

  res.code = '2.04';
  res.end(PONG_PAYLOAD);
  console.log(`Response peer=${req.rsinfo.address}:${req.rsinfo.port} code=2.04 bytes=4`);
}

function onListening(error) {
  if (error) {
    onServerError(error);
    return;
  }
  console.log(`Listening on CoAP UDP ${BIND_ADDRESS}:${COAP_PORT}`);
}

function onServerError(error) {
  console.error(`CoAP server error: ${error.code || 'ERROR'}: ${error.message}`);
  process.exitCode = 1;
  if (!server_failed) {
    server_failed = true;
    server.close();
  }
}

function onResponseError(error) {
  console.error(`CoAP response error: ${error.code || 'ERROR'}: ${error.message}`);
}

server.on('request', onRequest);
server.on('error', onServerError);
server.listen(COAP_PORT, BIND_ADDRESS, onListening);
