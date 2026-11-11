# Architecture decision history

## ADR-001 — Minimal UDP baseline (2026-11-10, approved)

Use a single Node.js file with built-in dgram, udp4, default 0.0.0.0:56830, exact ASCII PING/PONG and replies to the observed sender. Configure the port with one source constant. This isolates basic UDP behavior without dependencies, persistent application state or future-phase functionality.

## ADR-002 — Initial organization (2026-11-10, superseded)

The original proposal used root-level source and registries. The first addendum replaced that layout with root src/, doc/, test/ and rls/ and branch Phase1-UF. These layouts were superseded before implementation by ADR-003; neither was implemented.

## ADR-003 — Phase directories and milestone history (2026-11-10, approved)

Supersedes ADR-002. The user selected phase1-udp-baseline as both development branch and phase directory, replacing Phase1-UF. Phase source and documentation reside under that directory; project-doc/ contains cross-phase records. README.md remains unchanged. Reserved test/ and rls/ contain only .gitkeep. This preserves identifiable experiments for future comparison.

Later phases use phase-specific branches from approved baselines and retain earlier phase directories. No future-phase directories are created now. The proposed annotated tag phase1-udp-baseline-v1.0 must await review and acceptance of the complete Phase 1 milestone. No Step 1 tag is created. Corrections must preserve existing acceptance tags and historical test evidence. No merge is authorized.


## ADR-004 — Step 2 approved G2 firmware design (2026-11-11)

G1 and complete G2 were explicitly approved before G3. Use the existing restored ESP-IDF v5.5.4 at commit 735507283d5b2f9fb363a1901172dbd9e847945d; no isolated installation is required after tracked SDK changes were reverted. Unrelated untracked SDK projects remain untouched.

Implement only app_main and wifi_event_handler in the approved nested ESP-IDF project. app_main is the sole connection owner, waits for STARTED, and uses one wrap-safe 30-second startup/IPv4 budget. FAILED is latched. Use bundled lwIP connected IPv4 UDP with at most 20 attempted exchanges, exact PING/PONG, two-second receive timeout, no retry/reconnect/custom task. Retain the existing server unchanged.

Stop Wi-Fi, unregister application callbacks and conditionally delete their event group before continuing guarded teardown. On failed unregistration, retain callback-reachable resources until reset. Private credentials are local ignored header macros and firmware outputs remain ignored. Physical 8 MB PSRAM is disabled; flash configured 16 MB QIO at 40 MHz; 40 MHz crystal; 4096-byte main stack; UART at 115200. G3 authorizes build only; G4 flashing/runtime tests need a separate explicit approval. No phase advancement, merge or tag.


## ADR-005 — Explicitly approved target recovery (2026-11-11)

After the G4 boot failure exposed a provisioned flash-encryption state, the user explicitly approved advancing SPI_BOOT_CRYPT_CNT from 0b001 to 0b011, a full-chip erase, and reflashing the already approved plaintext firmware. This is a deployment/hardware-state exception to the prior no-eFuse-change/no-whole-chip-erase baseline, not a firmware design change.

Preflight confirmed the counter was 0b001 and writable, Secure Boot was disabled, and the approved generated firmware had flash encryption disabled. Only the requested counter field was burned to 0b011; readback reported flash encryption disabled. The full-chip erase completed and all three firmware image writes were hash-verified. Other existing security state, including provisioned key purpose and permanently disabled JTAG, was not altered. The board subsequently booted ESP-IDF v5.5.4 and app_main successfully. Historical failed-boot evidence is retained.


## Step 3 completeness review and consolidated approved G2 record (2026-11-11)

This section consolidates already approved decisions and their final revisions; it does not introduce an ADR or new architectural decision. ADR-001 covers the minimal NodeJS baseline, ADR-002 retains superseded layouts, ADR-003 covers phase organization/tag authority, ADR-004 covers the final G2 firmware design, and ADR-005 records the explicitly approved deployment exception. The additional user-provided private SSID case correction changed only ignored configuration. The two AP-control tests were explicitly left unexecuted; their omission is a coverage decision, not a new transport design.

### Final approved design references

- Objective and files: existing cloud.js plus the nested ESP-IDF project, as listed in ../phase1-udp-baseline/doc/FILE_REGISTRY.md. No implementation for later phases.
- Custom functions: void app_main(void) and static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data). SDK calls app_main; the SDK default event loop invokes the callback. Full purposes, inputs, results and API ownership are in FUNCTION_REGISTRY.md.
- Variables: exact names/types/scopes/lifetimes/defaults in VARIABLE_REGISTRY.md include MAX_EXCHANGE_ATTEMPTS=20, counters, 64-byte receive buffer, STARTED/READY/FAILED bits, shared deadline ticks, socket and cleanup ownership handles/flags. Callback receives SDK-owned event data and retains no pointer to main's automatic storage.
- Dependencies: built-in ESP-IDF esp_wifi, esp_event, esp_netif, nvs_flash, lwip, freertos, log, esp_common and heap; standard C/socket headers and local private header. Complete includes/API list is in FUNCTION_REGISTRY.md. No external package or custom task/transport layer.
- Credentials: ignored .private/wifi_credentials.h defines TRIAL_WIFI_SSID/TRIAL_WIFI_PASSWORD string literals, required by component CMake. Validate lengths without truncation before copying; WIFI_STORAGE_RAM. Header, generated configuration and credential-bearing binaries remain excluded from Git. Complete non-secret effective configuration is preserved in EXPERIMENT_RESULTS.md.
- STARTED: callback sets BIT2 on WIFI_EVENT_STA_START. app_main is the only esp_wifi_connect caller, once after STARTED and with FAILED clear. STARTED/READY/FAILED waits do not clear event bits.
- READY/FAILED: initial zero; GOT_IP sets READY only if FAILED clear; disconnect clears READY and latches FAILED. A later IP event cannot resume the experiment. Recheck bits before send and after receive before accepting a success.
- Deadline: record xTaskGetTickCount immediately before esp_wifi_start; use unsigned elapsed subtraction, check exhaustion before budget subtraction. Both startup and IPv4 waits use the remaining portion of the same 30000 ms budget. API calls are not preempted by a separate timer; elapsed budget is checked after they return. Hardware tick rollover/full-deadline tests were not performed.
- UDP: one connected IPv4 UDP socket, peer 192.168.0.170:56830, 2-second receive timeout, exact four-byte PING/PONG. connect selects a peer without a handshake. Count an attempt just before send, including failed sends; count success only for exact PONG and valid Wi-Fi bits. At most 20 attempts, stop on first failure; one-second delay only between successful exchanges.
- Errors: log failed initialization stage/API error/errno; no NVS auto-erase, firmware reboot recovery, reconnect or UDP retransmission. Missing credentials fail build configuration. Unrecognized server inputs have no reply; unexpected firmware replies end the run. SDK-internal default-interface assertion behavior is not replaced by a project recovery framework.
- Cleanup: close socket, stop Wi-Fi if started, unregister both acquired callback instances with exact keys. If any unregister fails, keep event group until reset. Otherwise delete group and clear static handle. Continue guarded Wi-Fi deinit, interface destruction, default-loop deletion and NVS deinit only when dependencies permit. Record cleanup warnings separately; SDK ESP-NETIF/lwIP infrastructure remains until reset.
- Expected resources: one app-owned UDP socket and event group, existing main task with 4096-byte stack, four-byte payload arrays and 64-byte receive buffer; SDK Wi-Fi/lwIP/event tasks and buffers. No application PSRAM requirement. Expected facilities are not resource measurements; actual build/runtime tables are in EXPERIMENT_RESULTS.md.
- Approved tests: Step 1 T01-T08 and Step 2 S2-T01-T14 are preserved with inputs, expected results, actual observations and status in EXPERIMENT_RESULTS.md. Step 2 source review preceded explicit G4 authorization. S2-T11/T12 remain unexecuted by user direction; no new test was run for closure.

### Consolidated final PlantUML flow

This textual diagram consolidates the already approved final revisions. It supersedes the historical PNG as the current design reference without altering that historical artifact. Only app_main and wifi_event_handler are custom functions; all other API names are SDK, FreeRTOS, C-library or lwIP facilities. Grouped setup actions check each call in source order and jump to cleanup immediately on the first failure.

```plantuml
@startuml
title Approved Phase 1 Step 2 — final consolidated design
start
:SDK calls app_main();
:nvs_flash_init();
:esp_netif_init();
:esp_event_loop_create_default();
:xEventGroupCreate();
:esp_netif_create_default_wifi_sta();
:esp_event_handler_instance_register()
Wi-Fi ANY_ID and IP GOT_IP;
:esp_wifi_init();
:esp_wifi_set_storage(WIFI_STORAGE_RAM);
:Validate/copy private credentials;
:esp_wifi_set_mode(WIFI_MODE_STA);
esp_wifi_set_config();
if (Initialization succeeded?) then (yes)
  :Record wifi_wait_started_at;
esp_wifi_start();
  if (Start API succeeded and budget remains?) then (yes)
    :xEventGroupWaitBits()
STARTED or FAILED; remaining budget;
    note right
      SDK calls wifi_event_handler():
      STA_START -> STARTED
      GOT_IP -> READY only if FAILED clear
      DISCONNECTED -> clear READY, latch FAILED
      Callback never calls esp_wifi_connect().
    end note
    if (STARTED set, FAILED clear, budget remains?) then (yes)
      :app_main calls esp_wifi_connect() once;
      if (Connect API succeeded and budget remains?) then (yes)
        :xEventGroupWaitBits()
READY or FAILED; same remaining budget;
        if (READY set, FAILED clear, deadline met?) then (yes)
          partition "UDP — bundled lwIP BSD sockets API" {
            :socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            :setsockopt(SO_RCVTIMEO, 2 seconds);
            :inet_pton(); connect(); getsockname();
            if (All socket setup succeeded?) then (yes)
              while (attempted_exchanges < 20?) is (yes)
                if (FAILED set or READY clear?) then (yes)
                  :Log Wi-Fi failure;
                  break
                endif
                :Increment attempted_exchanges;
send() four-byte PING;
                if (Exactly 4 bytes sent?) then (yes)
                  :recv() into 64-byte buffer;
                  if (Receive succeeded, READY set, FAILED clear,
length=4 and exact PONG?) then (yes)
                    :Increment successful_exchanges;
log successful exchange;
                    if (More attempts remain?) then (yes)
                      :vTaskDelay(1000 ms);
                    endif
                  else (no)
                    :Log timeout/error/unexpected response;
                    break
                  endif
                else (no)
                  :Log send failure;
                  break
                endif
              endwhile (no)
            else (no)
              :Log socket setup failure;
            endif
          }
        else (no)
          :Log IPv4 failure/deadline expiry;
        endif
      else (no)
        :Log connect API error/deadline expiry;
      endif
    else (no)
      :Log startup failure/deadline expiry;
    endif
  else (no)
    :Log Wi-Fi start error/deadline expiry;
  endif
else (no)
  :Log initialization failure;
endif
:Shared cleanup in app_main();
Log counts, result and heap/stack observations;
:close() socket if acquired;
:esp_wifi_stop() if started;
:esp_event_handler_instance_unregister()
for each acquired application registration;
if (All acquired callbacks successfully unregistered?) then (yes)
  :vEventGroupDelete() if group exists;
wifi_events = NULL;
else (no)
  :Retain group until reset; log warning;
endif
:Continue ownership/dependency-guarded teardown:
esp_wifi_deinit();
esp_netif_destroy_default_wifi();
esp_event_loop_delete_default();
nvs_flash_deinit();
note right
  Do not free resources still reachable
  after a shutdown failure.
  Log cleanup warnings separately.
  No retries, extra tasks or reconnect.
end note
:Return from app_main(); SDK scheduler continues;
stop
@enduml
```

### Closure status

Step 3 is a documentation/source/artifact inspection. No new source, SDK changes, device security changes, builds, flashes, erases or hardware tests were performed. Recommend submitting the bounded baseline with disclosed coverage gaps for independent acceptance review; no milestone acceptance, tag, merge or Phase 2 implementation is decided here.
