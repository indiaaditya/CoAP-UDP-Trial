'use strict';

const dgram = require('node:dgram');

// Edit this constant to select the local UDP listening port.
const UDP_PORT = 56830;
const BIND_ADDRESS = '0.0.0.0';
const PING_PAYLOAD = Buffer.from('PING', 'ascii');
const PONG_PAYLOAD = Buffer.from('PONG', 'ascii');
const socket = dgram.createSocket('udp4');

socket.on('listening', () => {
  const address = socket.address();
  console.log(`Listening on UDP ${address.address}:${address.port}`);
});

socket.on('message', (message, rinfo) => {
  console.log(`Received from ${rinfo.address}:${rinfo.port} bytes=${message.length} payload=${JSON.stringify(message.toString('utf8'))}`);

  if (message.equals(PING_PAYLOAD)) {
    socket.send(PONG_PAYLOAD, rinfo.port, rinfo.address, (error) => {
      if (error) {
        console.error(`UDP send error to ${rinfo.address}:${rinfo.port}: ${error.code}: ${error.message}`);
        return;
      }
      console.log(`Sent PONG to ${rinfo.address}:${rinfo.port}`);
    });
  }
});

socket.on('error', (error) => {
  console.error(`UDP socket error: ${error.code}: ${error.message}`);
  process.exitCode = 1;
  socket.close();
});

socket.bind(UDP_PORT, BIND_ADDRESS);
