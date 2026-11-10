# Trial protocol registry

Introduced in Phase 1, Step 1. Raw UDP payloads without envelope or terminator.

| Direction | ASCII | Hex | Action |
| --- | --- | --- | --- |
| Client to cloud | PING | 50 49 4e 47 | Recognize exact four-byte datagram |
| Cloud to client | PONG | 50 4f 4e 47 | Reply to received source IPv4 address and UDP port |

Matching is byte-exact and case-sensitive. Extra bytes, newline, NUL and lowercase do not match. Unknown datagrams are logged without an application response. UTF-8 conversion and escaping affect console display only. No retries, heartbeat or additional messages are defined.
