# Phase 2 resource evidence - G3-B review candidate

G3-A accepted source/build commit: aad119d4a980b37e510940e37a85b835a1630717. Current G3-B changes are uncommitted; no push authorization. G3-C/G4 remain unauthorized. All values in this document are build/linker observations or explicitly identified calculations. No runtime CoAP/heap/stack/network observation has been collected.

## Evidence identities and comparison boundaries

- Historical source/document baseline: ac1fa50d6731fd3f607789862fa28fadd8bf84e4, phase1-udp-baseline. Phase1 source/configuration/evidence have not been modified or rebuilt.
- Retained Phase1 final .bin: 757920 bytes; SHA256 594f9b9ef0d0abc3d5ddc387f263380254bc862c25426ee3539a8334c1f8d762.
- Retained Phase1 map: SHA256 0eb32d04f99487624e9757fb7ca0628df67a6ba90a1e5a30392984eef1c94a5c; neighboring ELF SHA256 3f1a646aade782be38d87432081eeaf14480a33da3d2e6eda39cac9e03527e66. Both match previously committed attribution identities. New read-only parsing is explicitly an analysis of retained artifacts, not a new Phase1 clean build or runtime result.
- Accepted G3-A .bin: 906608 bytes; SHA256 c33d71b3f25865515c401873941dff54450011a351ab63dc3cc378535849924a. Map SHA256 880e8d6550f71271f8bab1cfd2443e42da01c00f55015d42de398f719d3d6720. Retained in build/, unchanged.
- Historical detailed size-section rows were originally recorded from an earlier build. Read-only parsing of the identity-verified retained final map here yields the same section counts; that does not retroactively create the missing historical B0 clean-build provenance. Historical runtime B3/B4 remain unavailable.

Tools: installed ESP-IDF esp_idf_size legacy JSON archive/file parser for continuity with the committed Phase1 attribution, and esp_idf_size.ng JSON2 for current memory-region/section/archive reports. Both operate on retained linked sections, not full unlinked archive file lengths. Raw reports remain in ignored build directories. Loaded flash contribution below includes code placed in RAM and initialized data; it excludes BSS and noload data. Some compiler-merged strings and linker fill cannot be assigned reliably to their original source owners.

## Explaining the 148688-byte G3-A binary increase

Whole measured file-size difference: 906608 - 757920 = **148688 bytes**. It includes the conventional component, supporting retained code/data, custom integration, existing essential logging, and image/linker effects. It is not libcoap-only code size.

| Retained loaded-section category | Phase1 bytes | G3-A bytes | Difference |
| --- | ---: | ---: | ---: |
| Flash .text | 539766 | 672342 | +132576 |
| Flash .rodata | 106816 | 122260 | +15444 |
| App descriptor | 256 | 256 | 0 |
| DIRAM .text | 74579 | 74579 | 0 |
| IRAM .text + vectors | 16384 | 16384 | 0 |
| DIRAM initialized .data | 19964 | 20636 | +672 |
| Loaded RTC slow section | 32 | 32 | 0 |
| Sum / size-tool image before binary padding | 757797 | 906489 | +148692 |
| Binary framing/padding remainder | 123 | 119 | -4 |
| Actual application .bin | 757920 | 906608 | +148688 |

BSS is not stored in the .bin: DIRAM BSS rises 15592 ->26936 (+11344). DIRAM data+BSS rises 35556 ->47572 (+12016). This is linked static storage, not runtime heap usage.

### Per-archive retention differences

| Archive | Phase1 loaded flash | G3-A loaded flash | Difference | DIRAM data+BSS difference |
| --- | ---: | ---: | ---: | ---: |
| libespressif__coap.a | 0 | 45112 | +45112 | +10948 |
| libmbedtls.a | 0 | 29463 | +29463 | +240 |
| libc.a | 39213 | 66985 | +27772 | +140 |
| libmbedcrypto.a | 61125 | 80698 | +19573 | +692 |
| libmbedx509.a | 0 | 9186 | +9186 | +0 |
| libesp_app_format.a | 59217 | 67553 | +8336 | +0 |
| liblwip.a | 91144 | 94752 | +3608 | -16 |
| libvfs.a | 5263 | 7120 | +1857 | +0 |
| libmain.a | 3636 | 4491 | +855 | +0 |
| libnet80211.a | 136797 | 137311 | +514 | +0 |
| libnvs_flash.a | 14959 | 15307 | +348 | +0 |
| libnewlib.a | 3201 | 3433 | +232 | +0 |
| libwpa_supplicant.a | 64089 | 64285 | +196 | +0 |
| libgcc.a | 0 | 121 | +121 | +0 |
| libesp_driver_uart.a | 7719 | 7815 | +96 | +0 |
| libesp_hw_support.a | 33197 | 33265 | +68 | +0 |
| libesp_vfs_console.a | 689 | 721 | +32 | +0 |
| libesp_partition.a | 1948 | 1924 | -24 | +0 |
| libpp.a | 63357 | 63337 | -20 | +0 |
| libesp_mm.a | 3590 | 3606 | +16 | +0 |
| libstdc++.a | 1460 | 1476 | +16 | +0 |
| libesp_coex.a | 318 | 334 | +16 | +0 |
| libphy.a | 36089 | 36103 | +14 | +0 |
| libesp_event.a | 4032 | 4040 | +8 | +0 |
| libhal.a | 17112 | 17104 | -8 | +0 |
| libspi_flash.a | 16161 | 16169 | +8 | +0 |
| libesp_netif.a | 6472 | 6464 | -8 | +0 |
| libesp_timer.a | 2885 | 2877 | -8 | +0 |
| libapp_update.a | 182 | 186 | +4 | +0 |
| libesp_driver_usb_serial_jtag.a | 2380 | 2384 | +4 | +0 |
| libcore.a | 287 | 283 | -4 | +0 |
| libesp_wifi.a | 5261 | 5257 | -4 | +0 |
| libfreertos.a | 20716 | 20720 | +4 | +0 |

Total archive-accounted loaded flash:749310 ->896693, difference147383. The difference between the complete loaded-section delta148692 and archive-accounted delta147383 is1309 bytes of section/alignment/unattributed accounting. Binary framing/padding changes by-4, producing the1305-byte residual relative to the .bin delta. This residual is kept explicit rather than assigned to CoAP.

Selected reconciliation: CoAP45112 + Mbed TLS29463 + crypto19573 + X5099186 + libc27772 + merged-rodata/app-format bucket8336 + lwIP3608 + VFS1857 + main855 + other archives1621 + binary/section-accounting residual1305 =148688.

Important attribution caveat: the app-format archive's +8336 is NOT an8336-byte growth in application metadata. The descriptor remains256 bytes. Map input .rodata.__esp_system_init_fn_init_show_app_info.str1.4 is compiler/linker merged string storage (G3-A size0x10526 versus pre-relaxation0xee); the parser assigns this pooled storage to that object. Strings from other components can therefore be undercounted in their own archive rows. Small archive differences also include alignment/literal relaxation. Archive values explain retention/accounting; they do not isolate marginal feature cost in a controlled ablation experiment.

### Security-backend retention without a security session

Mbed TLS-related archive loaded-flash increase:29463 +19573 +9186 = **58222 bytes**, with DIRAM static increase932 bytes (240 TLS +692 crypto). Crypto already occupied61125 loaded bytes in Phase1; attributing all80698 G3-A crypto bytes to CoAP would be wrong.

The CoAP archive itself includes coap_mbedtls.c.obj:4769 loaded flash bytes and1496 static bytes; these are subsets of the45112/10948 CoAP totals and must NOT be added again. Its code+rodata/data backend paths coexist with a plain COAP_PROTO_UDP application.

Map archive-inclusion evidence directly links coap_mbedtls.c.obj to mbedtls_ssl_init/get_record_expansion/ciphersuite, entropy/CTR-DRBG, PK/key parsing and X509 verification-info symbols. Defined ELF symbols confirm retained coap_dtls_new_context, coap_dtls_new_client_session, mbedtls_ssl_setup and mbedtls_x509_crt_parse code. Generic coap_new_context conditionally allocates backend context when backend support is compiled; this can also contribute to later B2/B3 initialization heap readings. It does not mean a DTLS handshake/session or certificate authentication was performed.

The official port defines COAP_WITH_LIBMBEDTLS from the already-enabled SDK CONFIG_MBEDTLS_TLS_ENABLED; its CMake requires mbedtls. Approved PSK/PKI options are off and application creates only UDP sessions. No custom adapter/global security setting change or optimization was attempted.

Other retained support examples: libc_a-strftime.o is pulled by coap_debug.c.obj, and formatting/scanning/time functions grow. CoAP object examples include coap_block.c.obj9206 loaded bytes, coap_net.c.obj7083, coap_pdu.c.obj6427, coap_debug.c.obj5177 and coap_session.c.obj4857. Internal block/debug support can remain linked despite excluded application use; object names do not establish enabled on-wire features or full debug logging. CoAP archive static storage is data572+BSS10376=10948 bytes.

## Instrumentation semantics and runtime availability

All project measurement variables are automatic storage inside app_main; five custom functions remain. Other four function bodies, exchange/pacing loop and CoAP/Wi-Fi teardown are text-identical to accepted G3-A.

| Checkpoint | Implemented placement | Runtime value now |
| --- | --- | --- |
| B0 | Separate clean primary/scoped binary and identity records | Build evidence below |
| B1 | Before NVS/network/Wi-Fi setup; internal8-bit free heap | Not Measured |
| B2 | After IPv4, before coap_startup/context/session | Not Measured |
| B3 | After session/token setup, immediately before first request construction | Not Measured |
| B4 | Scoped only: read while monitor active, before stop at campaign boundary | Not Measured; historical UDP unavailable |
| B5 | After campaign/abort and scoped stop, before CoAP cleanup | Not Measured |
| B6 | Main-task watermark at result stage before cleanup | Not Measured |
| Whole-run minimum | Primary only; unreset SDK minima | Not Measured |
| Task counts | Before library startup and after session/token setup | Not Measured |

Unreached B1/B2/B3 readings are never synthesized from zero-initialized locals. B4 explicitly unavailable if monitor was not started. Abort-path B5/B6 are labeled result-stage observations, not a completed20-transaction campaign. Task-count differences are observational and not definitive ownership attribution; no task is created by project code.

Scoped variant: sample heap before monitor; start; sample after; capture B3; campaign; capture B4; stop; capture B5/B6. Monitor bookkeeping remains allocated during B4. G4 can report before-minus-after monitor-start free heap as an observed delta, including allocator overhead/concurrent SDK activity. No precise zero-cost correction is claimed. SDK uses sizeof(size_t) times registered heap count for requested bookkeeping, plus allocator overhead; allocation failure can assert. Stop restores minima and frees bookkeeping. Scoped output deliberately omits historical whole-run watermark comparison. The API aggregate combines per-region minima, not necessarily simultaneous free memory.

Scope includes B3 logging/iteration logging and timing within monitored interval; startup/context init excluded from B4. Initialization effect is observed via primary B2/B3, including existing platform activity. B4 does not isolate library-owned allocations. No post-cleanup heap or all-SDK-task stack profile is claimed.

Historical comparison: R1/R2 B1=329724/329724; B2=278212/278200; B5=277576/277576; B6=1748/1748; whole-run minima275460/274008 bytes. Missing historical clean-build B0, B3 and B4 remain missing. Free-heap delta is CoAP free minus UDP free; occupied delta has opposite sign. Do not subtract unavailable rows or add heap/stack to linker regions as independent capacities. Main stack remains4096; occupancy is derived as4096 minus observed minimum when G4 exists, not guaranteed stack requirement. No Beken extrapolation.

## G3-B primary/scoped build evidence

See appended measured tables and EXPERIMENT_RESULTS.md for commands, exact source/config/artifact identities, warnings and checks. No runtime/network campaign has been run. Primary/scoped artifacts are separate from retained accepted G3-A build/. Further optimization or hardware deployment requires separate authorization.


Both clean builds completed successfully using fresh directories. Compiler warning/error scan of their saved logs found none. SDK/toolchain, dependencies and existing global settings match G3-A. Only CONFIG_TRIAL_SCOPED_HEAP_MONITOR differs between the variants (n vs y). No manifest/lockfile change.

| Binary/build property | Accepted G3-A | G3-B primary | G3-B scoped |
| --- | ---: | ---: | ---: |
| Application .bin | 906608 | 907040 | 907984 |
| Size-tool loaded image before binary framing/padding | 906489 | 906921 | 907861 |
| Framing/padding remainder | 119 | 119 | 123 |
| Bootloader .bin | 22272 | 22272 | 22272 |
| Factory app allocation | 1048576 | 1048576 | 1048576 |
| Remaining factory app bytes | 141968 | 141536 | 140592 |
| DIRAM .data + .bss (derived subtotal) | 47572 | 47572 | 47588 |

| Section/region (bytes) | Accepted G3-A | Primary | Scoped |
| --- | ---: | ---: | ---: |
| Flash Code aggregate | 672342 | 672566 | 673074 |
| Flash Code .text | 672342 | 672566 | 673074 |
| Flash Data aggregate | 122516 | 122708 | 123124 |
| Flash Data .rodata | 122260 | 122452 | 122868 |
| Flash Data .appdesc | 256 | 256 | 256 |
| DIRAM aggregate | 122151 | 122167 | 122183 |
| DIRAM .text | 74579 | 74595 | 74595 |
| DIRAM .bss | 26936 | 26936 | 26936 |
| DIRAM .data | 20636 | 20636 | 20652 |
| IRAM aggregate | 16384 | 16384 | 16384 |
| IRAM .text | 15356 | 15356 | 15356 |
| IRAM .vectors | 1028 | 1028 | 1028 |
| RTC SLOW aggregate | 32 | 32 | 32 |
| RTC SLOW .force_slow | 32 | 32 | 32 |
| RTC FAST aggregate | 24 | 24 | 24 |
| RTC FAST .rtc_reserved | 24 | 24 | 24 |

Region aggregates include their children; never add parent and child values. DIRAM includes resident code, not only static variables. IRAM100% is the specified16384-byte linker region, not all chip RAM.

### Instrumentation build overhead

Primary versus G3-A:+432 binary bytes =+224 flash text,+192 flash rodata,+16 DIRAM resident code. No .data/BSS increase. Main-object loaded flash4491 ->4711 (+220); retained support and pooled/alignment effects explain the rest. This is the incremental primary instrumentation build cost relative to the accepted G3-A build, not total measurement overhead versus an uninstrumented application (G3-A already logged essential checkpoints).

Scoped versus primary:+944 binary bytes. Loaded-section increase940 =+508 flash text,+416 rodata,+16 initialized data; binary framing/padding adds4. DIRAM BSS unchanged. Heap-library loaded contribution grows525 loaded bytes (461 text+48 rodata+16 data); main grows64; other differences include compiler-merged strings/alignment/relaxation. No claim that all944 bytes are SDK monitor code.

SDK monitor state contributes16 additional static data bytes in libheap.a (data4 ->20; BSS8 unchanged); bookkeeping allocation and task-stack high-water impact remain Not Measured. Locals have automatic storage; static totals do not quantify their runtime stack use. Full unchanged main stack allocation is4096 bytes in both variants.

Bootloader sizes are unchanged. Primary/scoped bootloader byte comparisons show only time-string bytes (12:07:06 vs12:11:20) and ensuing checksum/SHA256 differences; no other .bin bytes differ. Keep distinct hashes rather than treating binaries as identical. The retained G3-A bootloader is also preserved separately.

### Current component attribution

Loaded flash column includes Flash Code/Data, RAM-resident initialized code and .data, excluding BSS/noload/reserved-only memory. DIRAM static columns are data+BSS only, excluding resident code. Attribution inherits merged-string and alignment limitations discussed above; it is not an independent marginal-library measurement.

| Component/archive | Primary loaded flash | Scoped loaded flash | Primary DIRAM static | Scoped DIRAM static |
| --- | ---: | ---: | ---: | ---: |
| libnet80211.a | 137311 | 137311 | 8538 | 8538 |
| liblwip.a | 94752 | 94752 | 2482 | 2482 |
| libmbedcrypto.a | 80698 | 80694 | 1032 | 1032 |
| libesp_app_format.a | 67741 | 68101 | 10 | 10 |
| libc.a | 66985 | 66985 | 724 | 724 |
| libwpa_supplicant.a | 64285 | 64281 | 1338 | 1338 |
| libpp.a | 63337 | 63337 | 3874 | 3874 |
| libespressif__coap.a | 45112 | 45108 | 10948 | 10948 |
| libphy.a | 36103 | 36103 | 3358 | 3358 |
| libesp_hw_support.a | 33297 | 33285 | 712 | 712 |
| libmbedtls.a | 29467 | 29467 | 240 | 240 |
| libfreertos.a | 20733 | 20733 | 3865 | 3865 |
| libhal.a | 17104 | 17104 | 3831 | 3831 |
| libspi_flash.a | 16098 | 16098 | 2180 | 2180 |
| libnvs_flash.a | 15307 | 15307 | 24 | 24 |
| libesp_system.a | 14626 | 14626 | 742 | 742 |
| libheap.a | 11588 | 12113 | 12 | 28 |
| libmbedx509.a | 9186 | 9186 | 0 | 0 |
| libesp_driver_uart.a | 7815 | 7811 | 336 | 336 |
| libvfs.a | 7120 | 7120 | 236 | 236 |
| libesp_netif.a | 6464 | 6468 | 33 | 33 |
| libesp_wifi.a | 5257 | 5257 | 523 | 523 |
| libmain.a | 4711 | 4775 | 4 | 4 |
| libesp_event.a | 4040 | 4040 | 4 | 4 |
| libxtensa.a | 3709 | 3709 | 1060 | 1060 |
| libesp_mm.a | 3606 | 3606 | 135 | 135 |
| libnewlib.a | 3433 | 3433 | 372 | 372 |
| libesp_phy.a | 3238 | 3238 | 52 | 52 |
| libesp_timer.a | 2877 | 2877 | 68 | 68 |
| libesp_driver_usb_serial_jtag.a | 2384 | 2384 | 73 | 73 |
| libesp_driver_gpio.a | 2158 | 2158 | 36 | 36 |
| libsoc.a | 2029 | 2029 | 0 | 0 |
| libesp_partition.a | 1924 | 1924 | 8 | 8 |
| liblog.a | 1597 | 1597 | 284 | 284 |
| libesp_common.a | 1865 | 1865 | 0 | 0 |
| libstdc++.a | 1476 | 1476 | 21 | 21 |
| libefuse.a | 1483 | 1483 | 40 | 40 |
| libesp_ringbuf.a | 1332 | 1332 | 0 | 0 |
| libbootloader_support.a | 1113 | 1113 | 0 | 0 |
| libesp_vfs_console.a | 721 | 721 | 20 | 20 |
| libpthread.a | 556 | 556 | 12 | 12 |
| libesp_rom.a | 536 | 536 | 4 | 4 |
| libesp_security.a | 484 | 484 | 8 | 8 |
| libxt_hal.a | 437 | 437 | 0 | 0 |
| libesp_coex.a | 334 | 334 | 0 | 0 |
| libcore.a | 283 | 283 | 9 | 9 |
| libapp_update.a | 186 | 186 | 4 | 4 |
| libgcc.a | 121 | 121 | 0 | 0 |
| libcxx.a | 52 | 52 | 0 | 0 |
| libnvs_sec_provider.a | 5 | 5 | 0 | 0 |
| libespnow.a | 3 | 3 | 3 | 3 |

### Artifact identity ledger

Paths below are relative to the firmware project build-primary/build-scoped roots. All binaries/ELF/maps/config/logs remain ignored local files because firmware contains private Wi-Fi data. No artifact is published or staged.

| Artifact | Bytes | SHA256 |
| --- | ---: | --- |
| primary/esp32_coap_client.bin | 907040 | 4102f0a46436431c9493555a4980e18d308eac40a541bb0fcb1669cda11ac8a9 |
| primary/esp32_coap_client.elf | 10071472 | 1ee47ba8c752d1cbddd0fb01899a330e85c775807c5fa297918868bc82e12533 |
| primary/esp32_coap_client.map | 8837869 | e6041aca172fee328433306ff02967c4c93736df63d190b2ce2d828f0e1e927f |
| primary/bootloader/bootloader.bin | 22272 | e278d249ab684203934f1d294a0917a2ac26becb1e0173f4e6ed51ed34b1eef4 |
| primary/sdkconfig | 76669 | 3010c3c7901f1744c9027abb1ce1145de9ef25814f1d3f679a2429b751de01e9 |
| scoped/esp32_coap_client.bin | 907984 | a17d628714b87614c1087b2e4fdcc7657a647bffd72c3fe120909b46f0248507 |
| scoped/esp32_coap_client.elf | 10076632 | 4de3133ddf3c0b830da4174773b818bc48d3ee8a88f4b3248e33cc631ca131c2 |
| scoped/esp32_coap_client.map | 8838830 | 9caf7be0a34311733130ea70caafde0c3138cb3cc4ec4250880000a9860631ea |
| scoped/bootloader/bootloader.bin | 22272 | f833d6eb13a00a9b07ddae004428a1521c204349b018b025222b77770e5f5338 |
| scoped/sdkconfig | 76658 | c05ab44f8fa418b518ce44170ea58508141763cb7f1848570cebde8278ba29e2 |

| Source/config/lock input | SHA256 |
| --- | --- |
| main/main.c | 650ff2b916d6ced6f6128689e778bf034418d9076ec17010a279d574ab72d818 |
| main/Kconfig.projbuild | 1e7741d84a65325e825ef98a90fabbc03a8abe0a1708fe9a7fa1acc060bade6b |
| sdkconfig.defaults | 7116faba9c82875b197183732ccace51e2b23a3f09e2bd164231f84d5fda2a2c |
| sdkconfig.scoped.defaults | e60aa5c1313794974e6c79470c59a6c0b443475b9dc9da8314753152a7fdb572 |
| dependencies.lock | 41c9cbe30c987381fe9e0531fe2d6420b93d6f0f8f22c680583fe36a9ec5b4c8 |

Exact build commands and scope controls are in EXPERIMENT_RESULTS.md. No runtime value or claim of hardware feasibility follows from these build results.
