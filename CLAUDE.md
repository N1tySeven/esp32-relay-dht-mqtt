# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Upload

```bash
# Build only
pio run

# Build + upload (change COM9 in platformio.ini to match your actual port first)
pio run --target upload

# Serial monitor
pio device monitor
```

Change `upload_port` and `monitor_port` in [platformio.ini](platformio.ini) to match the ESP32's COM port before uploading.

## First-time Setup (credentials)

`include/secrets.h` and `src/secrets.cpp` are gitignored. Copy the example templates:

```powershell
Copy-Item include\secrets.h.example include\secrets.h
Copy-Item src\secrets.cpp.example   src\secrets.cpp
```

Then edit `src/secrets.cpp` with real WiFi credentials. `secrets.h` contains only `extern` declarations — no values to change there.

## Architecture

```
include/config.h       — pin #defines + extern declarations for MQTT constants
include/mqtt_handler.h — extern WiFiClient/PubSubClient/DHT + function declarations
include/secrets.h      — extern declarations for WiFi credentials (gitignored)
src/config.cpp         — definitions of MQTT constants (server, topics, port)
src/secrets.cpp        — definitions of WiFi credentials (gitignored)
src/mqtt_handler.cpp   — all logic: WiFi setup, MQTT callback, DHT publish, reconnect
src/main.cpp           — setup() / loop() only; no logic here
```

**Extern pattern**: every global object is declared `extern` in the header and defined in exactly one `.cpp`. Never define globals in headers — it causes multiple-definition linker errors on Arduino/PlatformIO.

**Non-blocking reconnect**: `reconnect()` uses `static unsigned long lastAttempt` + `millis()` with a 5-second retry interval. Never put `while(!client.connected())` blocking loops in this codebase.

**DHT publish**: `dht_publish()` uses the same `static unsigned long lastPublish` + `millis()` pattern (10-second interval). Called every loop iteration; returns immediately if interval hasn't elapsed or client is disconnected.

**Unique MQTT client ID**: derived from MAC address via `WiFi.macAddress()` — format `ESP32-AA:BB:CC:DD:EE:FF`.

## Code Conventions

- Use `char[]` + `memcpy`/`strcmp`/`snprintf` — never `String` class (heap fragmentation on ESP32).
- Use `strcmp(topic, mqtt_topic_xxx) == 0` for exact topic matching — never `strstr`/`indexOf`.
- Build JSON payloads with `snprintf` — ArduinoJson is not a dependency and must not be added.
- Relay callback accepts only `"ON"` (HIGH) and `"OFF"` (LOW) — exact case, no aliases.

## MQTT Topics

| Topic | Direction | Notes |
|-------|-----------|-------|
| `relay_control` | Subscribe | Relay on GPIO 18 |
| `relay_control_2` | Subscribe | Relay on GPIO 19 |
| `DHT11` | Publish | `{"temperature":xx.x,"humidity":xx.x}` every 10 s |
| `LDR` | Publish | `"BRIGHT"` or `"DARK"` — state-change only, checked every 2 s |

Broker: `broker.hivemq.com:1883` — public, no TLS. Defined in `src/config.cpp`.

**LDR sensor**: GPIO 34 (digital input, 3-pin DO module). LOW = bright, HIGH = dark. `ldr_publish()` only sends when state changes (uses `static int lastState`).
