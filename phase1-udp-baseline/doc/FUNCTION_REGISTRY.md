# Function and callback registry

No named function declarations or wrappers are introduced. All callbacks below are anonymous, in phase1-udp-baseline/src/cloud.js, introduced in Phase 1, Step 1. Return values are unused (undefined).

| Callback | Purpose | Inputs | Outputs / effects | Trigger |
| --- | --- | --- | --- | --- |
| listening | Report bound endpoint | None; reads socket.address() | Console listening line | Socket listening event |
| message | Log packet; recognize exact PING | message: Buffer; rinfo: sender metadata | Reception log; send PONG to sender on match | Socket message event |
| send completion | Report completion or send error | error: Error or null | Destination line or error details; no retry | socket.send completion |
| error | Report bind/runtime error and terminate | error: Error | Error code/message; exitCode=1; socket.close() | Socket error event |

Successful send completion does not establish delivery. Client observations establish receipt. Ctrl+C uses default Node.js termination without a registered shutdown callback.
