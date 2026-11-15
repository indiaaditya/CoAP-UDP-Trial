'use strict';

// Test-only program. Run only during an explicitly authorized network test.
// The npm manifest lives in ../src; use its installed public codec package.
const dgram = require('node:dgram');
const coapPacket = require('../src/node_modules/coap-packet');
const FRONT_PORT = 56830;
const BACKEND_ADDRESS = '127.0.0.1';
const BACKEND_PORT = 56831;
const SEPARATE_DELAY_MS = 250;
const DUPLICATE_DELAY_MS = 250;
const FIXTURE_TIMEOUT_MS = 180000;
const MAX_CAPTURED_DATAGRAMS = 256;

let mode = process.argv[2];
let payload_case = process.argv[3];
let front = null;
let back = null;
let wrong_port_socket = null;
let client = null;
let request_count = 0;
let response_count = 0;
let first_request = null;
let first_response = null;
const scheduled_timers = new Set();
let watchdog = null;
let ended = false;
const capture = [];
let probe_request = null;
let probe_expected = null;
let probe_sent = false;

function main() {
  if (!['pass', 'drop-first-request', 'drop-first-response', 'duplicate-request',
    'duplicate-response', 'blackhole', 'separate', 'separate-duplicate',
    'ack-only', 'wrong-token', 'wrong-code', 'bad-payload', 'malformed',
    'wrong-port', 'rst', 'probe'].includes(mode) || process.argv.length > 4) {
    finish(false, 'usage: node test/coap_fixture.js MODE [CASE]');
    return;
  }
  if (mode === 'bad-payload') {
    payload_case = payload_case || 'FAIL';
    if (!['FAIL', 'LF', 'OVERSIZE'].includes(payload_case)) {
      finish(false, 'bad-payload CASE must be FAIL, LF or OVERSIZE');
      return;
    }
  } else if (mode === 'probe') {
    payload_case = payload_case || 'valid';
    if (!['valid', 'path', 'method', 'payload', 'format'].includes(payload_case)) {
      finish(false, 'probe CASE must be valid, path, method, payload or format');
      return;
    }
    probe_request = {
      confirmable: true, ack: false, reset: false,
      code: payload_case === 'method' ? '0.01' : '0.02', messageId: 42,
      token: Buffer.from([0x80, 0, 0, 42]),
      options: [{ name: 'Uri-Path', value: Buffer.from(payload_case === 'path' ? 'missing' : 'coap-test') }],
      payload: Buffer.from(payload_case === 'payload' ? 'FAIL' : 'PING')
    };
    if (payload_case !== 'format') {
      probe_request.options.push({ name: 'Content-Format', value: Buffer.alloc(0) });
    }
    probe_expected = {
      code: payload_case === 'valid' ? '2.04' : payload_case === 'path' ? '4.04'
        : payload_case === 'method' ? '4.05' : '4.00',
      payload: payload_case === 'valid' ? Buffer.from('PONG') : Buffer.alloc(0),
      accepted: false
    };
  } else if (payload_case !== undefined) {
    finish(false, 'CASE is supported only for bad-payload and probe');
    return;
  }

  try {
    front = dgram.createSocket('udp4');
    front.on('message', onFrontMessage);
    front.on('error', onSocketError);
    front.on('listening', onListening);
    if (['pass', 'drop-first-request', 'drop-first-response',
      'duplicate-request', 'duplicate-response'].includes(mode)) {
      back = dgram.createSocket('udp4');
      back.on('message', onBackMessage);
      back.on('error', onSocketError);
      back.on('listening', onListening);
      back.bind(0, BACKEND_ADDRESS);
    }
    if (mode === 'wrong-port') {
      wrong_port_socket = dgram.createSocket('udp4');
      wrong_port_socket.on('error', onSocketError);
      wrong_port_socket.on('listening', onListening);
      wrong_port_socket.bind(0, '0.0.0.0');
    }
    watchdog = setTimeout(onTimeout, FIXTURE_TIMEOUT_MS);
    process.on('SIGINT', onSignal);
    process.on('SIGTERM', onSignal);
    front.bind(mode === 'probe' ? 0 : FRONT_PORT, mode === 'probe' ? BACKEND_ADDRESS : '0.0.0.0');
    console.log(`FIXTURE mode=${mode} case=${payload_case || '-'}; wait for all socket listening lines`);
  } catch (error) {
    finish(false, `startup: ${error.message}`);
  }
}

function onFrontMessage(message, rinfo) {
  if (ended) return;
  if ((mode === 'probe' && (rinfo.address !== BACKEND_ADDRESS || rinfo.port !== FRONT_PORT)) ||
      (client !== null && (rinfo.address !== client.address || rinfo.port !== client.port))) {
    console.log(`IGNORED unexpected front peer=${rinfo.address}:${rinfo.port}`);
    return;
  }
  if (capture.length >= MAX_CAPTURED_DATAGRAMS) {
    finish(false, 'capture-limit');
    return;
  }
  try {
    const packet = coapPacket.parse(message);
    capture.push({ direction: 'front-in', time: Date.now(), length: message.length,
      mid: packet.messageId, token: packet.token.toString('hex'), code: packet.code, action: mode });
    console.log(JSON.stringify(capture[capture.length - 1]));

    if (mode === 'probe') {
      if (packet.code === '0.00' && packet.ack && packet.messageId === probe_request.messageId &&
          packet.token.length === 0 && packet.payload.length === 0 && packet.options.length === 0) {
        console.log('PROBE empty ACK: awaiting application response');
        return;
      }
      if (packet.reset || (!packet.ack && !packet.confirmable) ||
          (packet.ack && packet.messageId !== probe_request.messageId) ||
          !packet.token.equals(probe_request.token) || packet.code !== probe_expected.code ||
          !packet.payload.equals(probe_expected.payload)) {
        finish(false, 'probe response mismatch');
        return;
      }
      let content_format = false;
      for (const option of packet.options) {
        if (option.name === 'Content-Format' && option.value.length === 0) content_format = true;
        if (['Observe', 'Block1', 'Block2'].includes(option.name)) {
          finish(false, 'probe unexpected response option');
          return;
        }
      }
      if (!content_format) {
        finish(false, 'probe missing text/plain content format');
        return;
      }
      if (packet.confirmable) {
        // Finish only after the four-byte ACK send callback, not after queuing it.
        probe_expected.accepted = true;
        sendDatagram(front, coapPacket.generate({ code: '0.00', messageId: packet.messageId,
          ack: true, confirmable: false, reset: false, token: Buffer.alloc(0),
          options: [], payload: Buffer.alloc(0) }), rinfo.port, rinfo.address);
      } else {
        finish(true, 'probe matched expected response');
      }
      return;
    }

    if (client === null) client = { address: rinfo.address, port: rinfo.port };
    ++request_count; // Front ingress datagrams, not logical device attempts.
    if (back !== null) {
      if (first_request === null) first_request = Buffer.from(message);
      if (mode === 'drop-first-request' && request_count === 1) {
        console.log('DROP first request');
        return;
      }
      sendDatagram(back, message, BACKEND_PORT, BACKEND_ADDRESS);
      if (mode === 'duplicate-request' && request_count === 1 && !ended) {
        scheduled_timers.add(setTimeout(sendScheduledDatagram, DUPLICATE_DELAY_MS,
          back, Buffer.from(message), BACKEND_PORT, BACKEND_ADDRESS));
      }
      return;
    }
    if (packet.code === '0.00' && (packet.ack || packet.reset)) {
      if (first_response !== null && packet.messageId === coapPacket.parse(first_response).messageId) {
        console.log(`CONTROL ${packet.ack ? 'ACK' : 'RST'} mid=${packet.messageId}`);
      }
      return;
    }
    if (!packet.confirmable || packet.ack || packet.reset || packet.code !== '0.02' ||
        packet.token.length !== 4 || !packet.payload.equals(Buffer.from('PING'))) {
      finish(false, 'synthetic mode requires trial CON POST with four-byte token and PING');
      return;
    }
    if (mode === 'blackhole') {
      console.log('DROP blackhole request');
      return;
    }
    const repeated = first_request !== null && first_request.equals(message);
    const empty_ack = coapPacket.generate({ code: '0.00', ack: true, confirmable: false,
      reset: false, messageId: packet.messageId, token: Buffer.alloc(0),
      options: [], payload: Buffer.alloc(0) });
    if (mode === 'ack-only') {
      sendDatagram(front, empty_ack, client.port, client.address);
      return;
    }
    if (!repeated) {
      first_request = Buffer.from(message);
      const reply = { code: mode === 'wrong-code' ? '4.00' : '2.04',
        ack: true, confirmable: false, reset: false, messageId: packet.messageId,
        token: Buffer.from(packet.token), options: [{ name: 'Content-Format', value: Buffer.alloc(0) }],
        payload: Buffer.from('PONG') };
      if (mode === 'wrong-token') reply.token[0] ^= 1;
      if (mode === 'bad-payload') {
        reply.payload = payload_case === 'LF' ? Buffer.from('PONG\n')
          : payload_case === 'OVERSIZE' ? Buffer.alloc(100, 0x58) : Buffer.from('FAIL');
      }
      if (mode === 'rst') {
        reply.code = '0.00'; reply.ack = false; reply.reset = true;
        reply.token = Buffer.alloc(0); reply.options = []; reply.payload = Buffer.alloc(0);
      }
      if (mode === 'separate' || mode === 'separate-duplicate') {
        reply.ack = false; reply.confirmable = true;
        reply.messageId = (packet.messageId + 1) & 0xffff;
      }
      first_response = coapPacket.generate(reply);
    }
    if (mode === 'separate' || mode === 'separate-duplicate') {
      sendDatagram(front, empty_ack, client.port, client.address);
      if (!repeated && !ended) {
        scheduled_timers.add(setTimeout(sendScheduledDatagram, SEPARATE_DELAY_MS,
          front, Buffer.from(first_response), client.port, client.address));
        if (mode === 'separate-duplicate') {
          scheduled_timers.add(setTimeout(sendScheduledDatagram, SEPARATE_DELAY_MS + DUPLICATE_DELAY_MS,
            front, Buffer.from(first_response), client.port, client.address));
        }
      }
    } else if (mode === 'malformed') {
      // Three bytes cannot form a CoAP header. Valid response follows at 250 ms.
      sendDatagram(front, Buffer.from([0x60, 0x44, 0]), client.port, client.address);
      if (!ended) scheduled_timers.add(setTimeout(sendScheduledDatagram, SEPARATE_DELAY_MS,
        front, Buffer.from(first_response), client.port, client.address));
    } else {
      sendDatagram(mode === 'wrong-port' ? wrong_port_socket : front,
        first_response, client.port, client.address);
    }
  } catch (error) {
    finish(false, `front packet: ${error.message}`);
  }
}

function onBackMessage(message, rinfo) {
  if (ended) return;
  if (rinfo.address !== BACKEND_ADDRESS || rinfo.port !== BACKEND_PORT || client === null) {
    console.log(`IGNORED unexpected backend peer=${rinfo.address}:${rinfo.port}`);
    return;
  }
  if (capture.length >= MAX_CAPTURED_DATAGRAMS) {
    finish(false, 'capture-limit');
    return;
  }
  try {
    const packet = coapPacket.parse(message);
    capture.push({ direction: 'back-in', time: Date.now(), length: message.length,
      mid: packet.messageId, token: packet.token.toString('hex'), code: packet.code, action: mode });
    console.log(JSON.stringify(capture[capture.length - 1]));
    ++response_count;
    if (first_response === null) first_response = Buffer.from(message);
    if (mode === 'drop-first-response' && response_count === 1) {
      console.log('DROP first response');
      return;
    }
    sendDatagram(front, message, client.port, client.address);
    if (mode === 'duplicate-response' && response_count === 1 && !ended) {
      scheduled_timers.add(setTimeout(sendScheduledDatagram, DUPLICATE_DELAY_MS,
        front, Buffer.from(message), client.port, client.address));
    }
  } catch (error) {
    finish(false, `backend packet: ${error.message}`);
  }
}

function sendDatagram(socket, message, port, address) {
  if (ended) return;
  if (capture.length >= MAX_CAPTURED_DATAGRAMS) {
    finish(false, 'capture-limit');
    return;
  }
  try {
    let packet = null;
    try { packet = coapPacket.parse(message); } catch { /* Deliberately malformed injection. */ }
    capture.push({ direction: socket === back ? 'back-out' : socket === wrong_port_socket ? 'wrong-port-out' : 'front-out',
      time: Date.now(), length: message.length, mid: packet === null ? null : packet.messageId,
      token: packet === null ? null : packet.token.toString('hex'),
      code: packet === null ? null : packet.code, action: mode });
    console.log(JSON.stringify(capture[capture.length - 1]));
    socket.send(message, port, address, onSendComplete);
  } catch (error) {
    finish(false, `send: ${error.message}`);
  }
}

function sendScheduledDatagram(socket, message, port, address) {
  if (!ended) sendDatagram(socket, message, port, address);
}

function onSendComplete(error, bytes) {
  if (ended) return;
  if (error) {
    finish(false, `send completion: ${error.message}`);
  } else {
    console.log(`SEND complete bytes=${bytes}; not proof of delivery`);
    if (mode === 'probe' && probe_expected.accepted && bytes === 4) {
      finish(true, 'probe matched response; CON ACK send completed');
    }
  }
}

function onSocketError(error) {
  finish(false, `socket: ${error.code || 'ERROR'}: ${error.message}`);
}

function onListening() {
  if (ended) return;
  const address = this.address();
  console.log(`LISTEN ${this === front ? 'front' : this === back ? 'back' : 'wrong-port'} ${address.address}:${address.port}`);
  if (mode === 'probe' && this === front && !probe_sent) {
    probe_sent = true;
    sendDatagram(front, coapPacket.generate(probe_request), FRONT_PORT, BACKEND_ADDRESS);
  }
}

function onTimeout() {
  finish(false, 'timeout');
}

function onSignal() {
  finish(true, 'operator-stop; not a test PASS');
}

function finish(passed, reason) {
  if (ended) return;
  ended = true;
  if (watchdog !== null) clearTimeout(watchdog);
  for (const timer of scheduled_timers) clearTimeout(timer);
  scheduled_timers.clear();
  process.removeListener('SIGINT', onSignal);
  process.removeListener('SIGTERM', onSignal);
  for (const socket of [front, back, wrong_port_socket]) {
    if (socket === null) continue;
    try { socket.close(); } catch (error) {
      if (error.code !== 'ERR_SOCKET_DGRAM_NOT_RUNNING') {
        passed = false;
        console.error(`Close error: ${error.message}`);
      }
    }
  }
  process.exitCode = passed ? 0 : 1;
  console.log(`FIXTURE ${reason === 'operator-stop; not a test PASS' ? 'STOPPED' : passed ? 'PASS' : 'FAIL'} reason=${reason} front_in=${request_count} back_in=${response_count} captured=${capture.length}`);
}

main();
