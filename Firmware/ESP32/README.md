# ESP32 Cloud Bridge — Firmware Design
**Status:** Design phase — not yet implemented  
**Target:** ESP32-WROOM-32  
**Framework:** Arduino (ESP32 Arduino Core v2.x+) or ESP-IDF with FreeRTOS

---

## Overview

The ESP32 acts purely as a **serial-to-cloud bridge**. It does not contain any irrigation logic — all decisions are made by the Sensor Module STM32. The ESP32's job is:

1. Maintain WiFi connectivity
2. Connect to MQTT broker and relay commands from the cloud to the STM32
3. Periodically poll the STM32 for sensor data and publish it to the cloud
4. Publish connection status events

---

## Hardware Connections

| ESP32 Pin | Connected To | Function |
|-----------|-------------|----------|
| GPIO16 (RX2) | Sensor Module PB10 (USART3 TX) | Receive STM32 telemetry |
| GPIO17 (TX2) | Sensor Module PB11 (USART3 RX) | Send commands to STM32 |
| GND | Sensor Module GND | Common ground |
| 3.3V | — | Powered from LM2596 5V via onboard regulator |

> Use `Serial2` (hardware UART2) in Arduino framework: `Serial2.begin(9600, SERIAL_8N1, 16, 17);`

---

## Required Libraries

```
PubSubClient      @ 2.8+   (MQTT client)
ArduinoJson       @ 6.x    (JSON parsing/serialization)
WiFi              (built-in ESP32 Arduino core)
time.h / NTP      (built-in, configTime())
```

---

## Architecture: Three FreeRTOS Tasks

```cpp
void setup() {
    xTaskCreate(wifiManagerTask,  "WiFiMgr",   4096, NULL, 1, NULL);
    xTaskCreate(mqttBridgeTask,   "MQTTBridge",8192, NULL, 2, NULL);
    xTaskCreate(stm32SerialTask,  "STM32Ser",  4096, NULL, 2, NULL);
}
```

### Task 1: WiFi Manager
```
- Connect to WiFi using stored SSID/password
- If connection lost: retry with exponential backoff (1s, 2s, 4s... up to 60s max)
- After successful connect: sync time via NTP (pool.ntp.org)
- Signal MQTT task via semaphore when WiFi is ready
```

### Task 2: MQTT Bridge
```
- Wait for WiFi semaphore
- Connect to broker: mqtts://{broker}:8883 (TLS)
- Subscribe: smartflow/{DEVICE_ID}/cmd
- On incoming message: parse JSON → translate to STM32 ASCII command → send via Serial2
- Publish status: smartflow/{DEVICE_ID}/status on connect/disconnect
- Reconnect loop if broker connection drops
```

### Task 3: STM32 Serial Task
```
- Every 10 seconds: send "999\n" to STM32 → read response → parse → publish JSON to sensors topic
- Receive STM32 responses into 256-byte buffer until '\n'
- Serialize sensor values to JSON payload
```

---

## Configuration (config.h)

```cpp
#define WIFI_SSID        "YourNetworkName"
#define WIFI_PASSWORD    "YourPassword"
#define MQTT_BROKER      "your-cluster.hivemq.cloud"
#define MQTT_PORT        8883
#define MQTT_USERNAME    "smartflow_user"
#define MQTT_PASSWORD    "your_mqtt_password"
#define DEVICE_ID        "sf2_001"
#define SENSOR_POLL_MS   10000   // poll STM32 every 10 seconds
```

---

## JSON ↔ STM32 Command Mapping

| MQTT JSON Payload | STM32 ASCII Command |
|------------------|---------------------|
| `{"action":"start_sequence"}` | `801\n` |
| `{"action":"set_duration","zone":1,"duration":600}` | `S1600\n` |
| `{"action":"set_duration","zone":2,"duration":900}` | `S2900\n` |
| `{"action":"set_duration","zone":3,"duration":1200}` | `S31200\n` |
| `{"action":"set_duration","zone":4,"duration":1800}` | `S41800\n` |
| `{"action":"set_tds_target","value":1.2}` | `E1200\n` |
| `{"action":"relay","channel":5,"state":true}` | `105\n` |
| `{"action":"relay","channel":5,"state":false}` | `005\n` |

---

## Sensor Payload Format

```json
{
  "device_id": "sf2_001",
  "tds": 1.24,
  "ph": 6.8,
  "pressure": 1.5,
  "pump_active": true,
  "sequence_state": 3,
  "timestamp": "2026-03-25T14:30:00Z"
}
```

Published to: `smartflow/sf2_001/sensors`

---

## Cloud Options

| Broker | Cost | Connections | Notes |
|--------|------|-------------|-------|
| HiveMQ Cloud (free tier) | Free | Up to 100 | 10 GB/month; good for prototype |
| Mosquitto on VPS | ~$5/month | Unlimited | Full control; requires sysadmin |
| AWS IoT Core | ~$1/million messages | Unlimited | Best for production scale |

**Prototype selection: HiveMQ Cloud** — zero infrastructure overhead, free tier sufficient for single-device testing.

---

## WiFi Reconnection Logic

```cpp
void reconnectWiFi() {
    int delay_ms = 1000;
    while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        vTaskDelay(delay_ms / portTICK_PERIOD_MS);
        delay_ms = min(delay_ms * 2, 60000);  // exponential backoff, cap at 60s
    }
    configTime(0, 0, "pool.ntp.org");  // NTP sync
}
```

---

## Implementation Status

- [ ] WiFi connect + NTP sync
- [ ] MQTT connect + TLS
- [ ] Subscribe to cmd topic
- [ ] JSON → STM32 command translation
- [ ] STM32 UART polling (999 command)
- [ ] Sensor JSON publish
- [ ] Status topic publish
- [ ] Reconnection logic (WiFi + MQTT)
- [ ] OTA firmware update (future)
