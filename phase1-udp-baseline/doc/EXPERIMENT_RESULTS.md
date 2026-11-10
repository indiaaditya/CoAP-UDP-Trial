# Step 1 experiment results

## Environment

Date: 2026-11-10 (Asia/Calcutta). Windows NT 10.0.22631.0, Node.js v25.9.0, win32 x64, PowerShell. Branch: phase1-udp-baseline. Starting commit: af4b50ef48305e17bdd77d4c899f0dbec1493cb5.

## Execution procedure

From the repository root:

1. Check Node.js: `node --version`.
2. Optional syntax check: `node --check phase1-udp-baseline/src/cloud.js`.
3. Start: `node phase1-udp-baseline/src/cloud.js`.
4. Confirm `Listening on UDP 0.0.0.0:56830`. Change UDP_PORT in the source to configure another port; update the client destination too. No compilation or package manager is required.
5. In a second PowerShell terminal, run this temporary single-request client with a 1000 ms timeout and no retries:

```powershell
@'
const client = require('node:dgram').createSocket('udp4');
const timer = setTimeout(() => {
  console.error('Timeout after 1000 ms');
  process.exitCode = 1;
  client.close();
}, 1000);
client.on('error', error => {
  clearTimeout(timer);
  console.error(error);
  process.exitCode = 1;
  client.close();
});
client.once('message', (data, sender) => {
  clearTimeout(timer);
  console.log('Local endpoint:', client.address());
  console.log('Response:', JSON.stringify(data.toString('utf8')), sender);
  if (!data.equals(Buffer.from('PONG', 'ascii'))) process.exitCode = 1;
  client.close();
});
client.send(Buffer.from('PING', 'ascii'), 56830, '127.0.0.1');
'@ | node
```

6. Observe exact PONG and compare the client endpoint with the server's reply destination. Stop with Ctrl+C in the server terminal. Interactive Ctrl+C confirmation remains outstanding as described below.

## Executed tests

A temporary command-line orchestration client used a bound localhost socket, exact Buffer assertions, 500 ms per-packet observation timeouts and no retries. No test script or output file was retained in the repository.

| Test | Actual observation | Result |
| --- | --- | --- |
| Syntax | node --check exited 0 | Passed |
| T01 | Bound 0.0.0.0:56830 | Passed |
| T02 | Received/logged PING from 127.0.0.1:54700, bytes=4 | Passed |
| T03 | Exact four-byte PONG received | Passed |
| T04 | Client endpoint 127.0.0.1:54700 matched logged reply destination; response source 127.0.0.1:56830 | Passed |
| T05 | 20 sequential exchanges; 20 successes, zero response timeouts or assertion failures | Passed |
| T06 | UNKNOWN, ping, PING+LF, PING+NUL, PING+0xff logged; no reply within 500 ms each | Passed |
| T07 | Repeated tool-injected Ctrl+C did not release socket; interactive termination not established | Inconclusive; manual confirmation required |
| T08 | Second server exited 1 with EADDRINUSE; original server answered another PING | Passed |

Recognized traffic: 21 requests, 21 exact responses, zero response timeouts or assertion failures. Unknown traffic: five requests, zero responses, five expected observation timeouts. A separate startup was used for T07. After Ctrl+C injection, a bind probe reported EADDRINUSE; the process was explicitly stopped and a fresh bind confirmed the port was reusable. Forced cleanup is not evidence of normal Ctrl+C termination.

## Representative captured logs

```text
Listening on UDP 0.0.0.0:56830
Client endpoint { address: '127.0.0.1', family: 'IPv4', port: 54700 }
Received from 127.0.0.1:54700 bytes=4 payload="PING"
Sent PONG to 127.0.0.1:54700
Received from 127.0.0.1:54700 bytes=7 payload="UNKNOWN"
Received from 127.0.0.1:54700 bytes=4 payload="ping"
UDP socket error: EADDRINUSE: bind EADDRINUSE 0.0.0.0:56830
Cleanup confirmed: port 56830 reusable
```

## Limitations

Only localhost was tested. Send-error and non-bind runtime-error branches were implemented but not fault-injected. T07 is inconclusive because the tool did not establish Ctrl+C delivery to Node.js. Process/network metadata queries were restricted. No LAN, embedded, CoAP, security or broader reliability conclusions follow. Phase 1 acceptance remains pending review and later work.
