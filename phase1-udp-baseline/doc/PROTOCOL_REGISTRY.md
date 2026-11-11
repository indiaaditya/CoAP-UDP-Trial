# Trial protocol registry

Introduced in Phase 1, Step 1. Raw UDP payloads without envelope or terminator.

| Direction | ASCII | Hex | Action |
| --- | --- | --- | --- |
| Client to cloud | PING | 50 49 4e 47 | Recognize exact four-byte datagram |
| Cloud to client | PONG | 50 4f 4e 47 | Reply to received source IPv4 address and UDP port |

Matching is byte-exact and case-sensitive. Extra bytes, newline, NUL and lowercase do not match. Unknown datagrams are logged without an application response. UTF-8 conversion and escaping affect console display only. No retries, heartbeat or additional messages are defined.


## Phase 1, Step 2 — embedded endpoint

No payload changes. ESP32 sends exactly four PING bytes to 192.168.0.170:56830. The existing cloud replies with four PONG bytes to the ESP32 source address/port. ESP32 uses a connected IPv4 UDP socket to restrict reception to that peer (no UDP handshake). Exact length and memcmp validation reject empty, malformed and longer data; no terminator is sent or expected.

Maximum 20 attempted UDP request/response exchanges. Increment attempts immediately before send; stop on the first failure. Successful responses require exact PONG and a current READY/no-FAILED snapshot. Receive timeout is two seconds; one-second delay only between successful exchanges. These are experimental execution rules, not new protocol messages, retry behavior, heartbeat or transaction management.


## Step 3 audit — 2026-11-11

Committed server/client byte arrays and comparisons match the two payloads above. NodeJS replies to received rinfo.address/rinfo.port; ESP32 transmits to its configured peer. Sender-directed behavior was observed in existing test evidence; no additional protocol was designed or tested for closure. Attempt counts, timeouts and pacing remain execution policy, not additional packet fields.
