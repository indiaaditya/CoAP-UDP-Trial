# Variable and constant registry

All are module-scoped const bindings in phase1-udp-baseline/src/cloud.js, introduced in Phase 1, Step 1. No persistent application state is introduced.

| Name | Type | Value / purpose |
| --- | --- | --- |
| dgram | Built-in module object | node:dgram API |
| UDP_PORT | number | 56830; edit to configure the listening port |
| BIND_ADDRESS | string | 0.0.0.0; all local IPv4 interfaces |
| PING_PAYLOAD | Buffer | ASCII PING, hex 50 49 4e 47; exact incoming match |
| PONG_PAYLOAD | Buffer | ASCII PONG, hex 50 4f 4e 47; response |
| socket | dgram.Socket | udp4 reception and transmission socket |

Routine callback-local address, message, rinfo and error values are described in the function registry and are not retained state.
