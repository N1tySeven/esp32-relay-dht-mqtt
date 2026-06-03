# ESP32 MQTT IoT Controller

ESP32 firmware (PlatformIO/Arduino) — controls two relays, publishes DHT11 sensor data and LDR light status via MQTT. Built for Node-RED dashboard integration.

## Hardware

| Component | Pin | Notes |
|-----------|-----|-------|
| Relay 1 | GPIO 18 | Active HIGH |
| Relay 2 | GPIO 19 | Active HIGH |
| DHT11 | GPIO 4 | Temperature & humidity |
| LDR (DO) | GPIO 34 | 3-pin digital module |
| Status LED | GPIO 2 | Built-in, lights on WiFi connect |

## Features

- **Dual relay control** via MQTT (`ON` / `OFF` only)
- **DHT11 sensor** publishes temperature & humidity as JSON every 10 seconds
- **LDR sensor** publishes light status (`BRIGHT` / `DARK`) on state change only, with retain flag
- **Non-blocking MQTT reconnect** with 5-second retry interval
- **Unique client ID** derived from MAC address
- **WiFi credentials** separated into gitignored `secrets.cpp`

## MQTT Topics

| Topic | Direction | Payload |
|-------|-----------|---------|
| `relay_control` | Subscribe | `ON` / `OFF` |
| `relay_control_2` | Subscribe | `ON` / `OFF` |
| `DHT11` | Publish | `{"temperature":30.5,"humidity":65.0}` every 10 s |
| `LDR` | Publish | `BRIGHT` or `DARK` — state-change only, retained |

Broker: `broker.hivemq.com:1883` (public, unencrypted)

## Project Structure

```
├── include/
│   ├── config.h          # Pin definitions + extern MQTT constant declarations
│   ├── mqtt_handler.h    # WiFiClient, PubSubClient, DHT extern + function declarations
│   ├── secrets.h         # extern declarations for WiFi credentials (gitignored)
│   └── secrets.h.example # Template — copy to secrets.h
├── src/
│   ├── main.cpp          # setup() / loop()
│   ├── mqtt_handler.cpp  # WiFi setup, MQTT callback, sensor publish, reconnect
│   ├── config.cpp        # MQTT constants definitions
│   ├── secrets.cpp       # WiFi credentials definitions (gitignored)
│   └── secrets.cpp.example # Template — copy to secrets.cpp
└── platformio.ini
```

## Getting Started

### 1. Clone and configure credentials

Change `upload_port` and `monitor_port` in `platformio.ini` to match your COM port, then:

```powershell
# Copy credential templates (Windows)
Copy-Item include\secrets.h.example include\secrets.h
Copy-Item src\secrets.cpp.example   src\secrets.cpp
```

Edit `src/secrets.cpp` with your WiFi credentials:

```cpp
const char* const ssid     = "YOUR_WIFI_SSID";
const char* const password = "YOUR_WIFI_PASSWORD";
```

### 2. Build and upload

```bash
pio run --target upload
pio device monitor
```

Or use the PlatformIO IDE extension in VS Code.

### 3. Monitor serial output

```
Connecting to YOUR_WIFI_SSID....
WiFi connected
IP address: 192.168.x.x
Attempting MQTT connection...connected to MQTT
DHT11 published: {"temperature":30.5,"humidity":65.0}
LDR published: BRIGHT
```

## Node-RED Integration

Two flow files are included:

| File | Description |
|------|-------------|
| `nodered_template_flow.json` | Full HTML dashboard served at `/ui` — no plugins required |
| `nodered_ldr_flow.json` | LDR-only flow using node-red-dashboard widgets |

Import via Node-RED → ☰ → Import. The template flow serves a dark-theme UI at `http://<node-red-ip>:1880/ui` with real-time temperature, humidity, LDR indicator, and relay buttons over WebSocket.

## Dependencies

| Library | Version |
|---------|---------|
| [PubSubClient](https://github.com/knolleary/pubsubclient) | ^2.8 |
| [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) | ^1.4.7 |

## Known Limitations

- Public broker (`broker.hivemq.com`) — **no TLS**, no authentication. Do not use in production without switching to port 8883 with `WiFiClientSecure`.
- Topics are not namespaced — prefix with your device MAC or project ID to avoid collisions on a shared broker.
- No WiFi reconnect after disconnect — device requires reboot if WiFi drops after initial connect.
- DHT11 accuracy: ±2°C / ±5% RH.
