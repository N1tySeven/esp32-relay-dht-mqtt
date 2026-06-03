# ESP32 MQTT IoT Controller

ESP32 firmware (PlatformIO/Arduino) — controls two relays and publishes DHT11 sensor data via MQTT. Built for Node-RED dashboard integration.

## Hardware

| Component | Pin |
|-----------|-----|
| Relay 1 | GPIO 18 |
| Relay 2 | GPIO 19 |
| DHT11 | GPIO 4 |
| Status LED | GPIO 2 |

## Features

- **Dual relay control** via MQTT (ON/OFF/1/0 commands)
- **DHT11 sensor** publishes temperature & humidity as JSON every 10 seconds
- **Non-blocking MQTT reconnect** with 5-second retry interval
- **Unique client ID** derived from MAC address
- **WiFi credentials** separated into gitignored `secrets.cpp`

## MQTT Topics

| Topic | Direction | Payload |
|-------|-----------|---------|
| `relay_control` | Subscribe | `ON` / `OFF` / `1` / `0` |
| `relay_control_2` | Subscribe | `ON` / `OFF` / `1` / `0` |
| `DHT11` | Publish | `{"temperature":30.5,"humidity":65.0}` |

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
│   ├── mqtt_handler.cpp  # WiFi setup, MQTT callback, DHT publish, reconnect
│   ├── config.cpp        # MQTT constants definitions
│   ├── secrets.cpp       # WiFi credentials definitions (gitignored)
│   └── secrets.cpp.example # Template — copy to secrets.cpp
└── platformio.ini
```

## Getting Started

### 1. Clone and configure credentials

```bash
git clone <repo-url>
cd Test_PlatformIO

# Copy templates
cp include/secrets.h.example include/secrets.h
cp src/secrets.cpp.example   src/secrets.cpp
```

Edit `src/secrets.cpp` with your WiFi credentials:

```cpp
const char* const ssid     = "YOUR_WIFI_SSID";
const char* const password = "YOUR_WIFI_PASSWORD";
```

### 2. Build and upload

```bash
# Install PlatformIO CLI first: https://docs.platformio.org/en/latest/core/installation/
pio run --target upload
pio device monitor
```

Or use the PlatformIO IDE extension in VS Code.

### 3. Monitor serial output

```
Connecting to YOUR_WIFI_SSID....
WiFi connected — IP: 192.168.x.x
Attempting MQTT connection...connected to MQTT
DHT11 published: {"temperature":30.5,"humidity":65.0}
```

## Node-RED Integration

Import the flow and connect MQTT nodes to the same broker. The DHT11 topic publishes JSON — set the MQTT In node **Output** to **"a parsed JSON object"** (or use a JSON node downstream) to feed the gauge nodes.

Expected payload structure:

```json
{ "temperature": 30.5, "humidity": 65.0 }
```

## Dependencies

| Library | Version |
|---------|---------|
| [PubSubClient](https://github.com/knolleary/pubsubclient) | ^2.8 |
| [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) | ^1.4.7 |

## Known Limitations

- Public broker (`broker.hivemq.com`) — **no TLS**, no authentication. Do not use in production or shared lab environments without switching to port 8883 with `WiFiClientSecure`.
- Topics are not namespaced — prefix with your device MAC or project ID to avoid collisions on a shared broker.
- DHT11 accuracy: ±2°C / ±5% RH.
