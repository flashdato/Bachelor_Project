# Communication Protocol Reference — Smart Flow 2.0

All inter-module communication uses ASCII strings over UART, newline-terminated (`\n`). No binary framing, no checksums — human-readable and debuggable with any serial terminal.

---

## B.1 — STM32 Inter-Module Protocol (Sensor Module → Relay Module)

**Transport:** USART1 on Sensor Module (PA9 TX) → USART3 on Relay Module (PB11 RX)  
**Baud rate:** 9600, 8N1  
**Format:** 3-digit ASCII decimal + `\n`

### Relay Commands

| Command | Relay # | Pin | Function |
|---------|---------|-----|----------|
| `001\n` | 1 OFF | PA2 → LOW | Main circulation pump — STOP |
| `101\n` | 1 ON | PA2 → HIGH | Main circulation pump — START |
| `002\n` | 2 OFF | PB5 → LOW | Fertilizer dosing pump — STOP |
| `102\n` | 2 ON | PB5 → HIGH | Fertilizer dosing pump — START |
| `003\n` | 3 OFF | PA3 → LOW | Zone 1 valve — CLOSE *(commented out)* |
| `103\n` | 3 ON | PA3 → HIGH | Zone 1 valve — OPEN *(commented out)* |
| `004\n` | 4 OFF | PA4 → LOW | Zone 2 valve — CLOSE *(commented out)* |
| `104\n` | 4 ON | PA4 → HIGH | Zone 2 valve — OPEN *(commented out)* |
| `005\n` | 5 OFF | PB4 → LOW | Zone 3 valve — CLOSE |
| `105\n` | 5 ON | PB4 → HIGH | Zone 3 valve — OPEN |
| `006\n` | 6 OFF | PB3 → LOW | Zone 4 valve — CLOSE |
| `106\n` | 6 ON | PB3 → HIGH | Zone 4 valve — OPEN |
| `007\n` | 7 OFF | PA15 → LOW | Zone 5 valve — CLOSE |
| `107\n` | 7 ON | PA15 → HIGH | Zone 5 valve — OPEN |
| `008\n` | 8 OFF | PA5 → LOW | Zone 6 valve — CLOSE |
| `108\n` | 8 ON | PA5 → HIGH | Zone 6 valve — OPEN |
| `009\n` | 9 OFF | PA12 → LOW | Zone 7 valve — CLOSE *(commented out)* |
| `109\n` | 9 ON | PA12 → HIGH | Zone 7 valve — OPEN *(commented out)* |
| `010\n` | 10 OFF | PA6 → LOW | Auxiliary 1 — OFF *(commented out)* |
| `110\n` | 10 ON | PA6 → HIGH | Auxiliary 1 — ON *(commented out)* |
| `011\n` | 11 OFF | PA7 → LOW | Auxiliary 2 — OFF *(commented out)* |
| `111\n` | 11 ON | PA7 → HIGH | Auxiliary 2 — ON *(commented out)* |

### Stepper Motor Commands

| Command | Action | Steps |
|---------|--------|-------|
| `M0\n` | Decrease fertilizer concentration | 0–90 steps (decrease direction) |
| `M1\n` | Increase fertilizer concentration | 0–90 steps (increase direction) |

---

## B.2 — ESP32 to Sensor Module Protocol

**Transport:** USART3 on Sensor Module (PB11 RX / PB10 TX)  
**Baud rate:** 9600, 8N1  
**Format:** ASCII string + `\n`; receive buffer = 100 bytes

| Command | Direction | Description |
|---------|-----------|-------------|
| `999\n` | ESP32 → Sensor | Request current sensor readings |
| *(response)* | Sensor → ESP32 | `TDS:x.xx\npH:x.x\nP:x.x\n` |
| `801\n` | ESP32 → Sensor | Start irrigation sequence |
| `S1xxxx\n` | ESP32 → Sensor | Set Zone 1 irrigation duration (seconds, 4-digit zero-padded) |
| `S2xxxx\n` | ESP32 → Sensor | Set Zone 2 irrigation duration |
| `S3xxxx\n` | ESP32 → Sensor | Set Zone 3 irrigation duration |
| `S4xxxx\n` | ESP32 → Sensor | Set Zone 4 irrigation duration |
| `Exxxx\n` | ESP32 → Sensor | Set user TDS target (target = value / 1000 in ppm) |
| `Rxx\n` | ESP32 → Sensor | Directly activate zone relay (relay index 3–11) |

**Example exchanges:**
```
ESP32 → STM32:  "999\n"
STM32 → ESP32:  "TDS:1.24\npH:6.80\nP:1.50\n"

ESP32 → STM32:  "S1600\n"    (Zone 1: 600 seconds = 10 minutes)
ESP32 → STM32:  "E1200\n"    (TDS target: 1200 / 1000 = 1.2 ppm target)
ESP32 → STM32:  "801\n"      (start irrigation sequence)
```

---

## B.3 — MQTT Topic and Payload Specification

**Broker:** HiveMQ Cloud (prototype) — configurable  
**Protocol:** MQTT 3.1.1 over TLS (port 8883)  
**Client ID:** `sf2_{device_id}`

### Topic: `smartflow/{device_id}/sensors`
**Direction:** ESP32 → Broker (published every 10 seconds)  
**QoS:** 1

```json
{
  "device_id": "sf2_001",
  "tds": 1.24,
  "ph": 6.8,
  "pressure": 1.5,
  "pump_active": true,
  "sequence": 3,
  "timestamp": "2026-03-25T14:30:00Z"
}
```

### Topic: `smartflow/{device_id}/cmd`
**Direction:** Mobile App → Broker → ESP32 (subscribed by ESP32)  
**QoS:** 1

```json
// Start irrigation sequence
{
  "action": "start_sequence",
  "zones": [3, 5, 6],
  "durations": [1800, 900, 1200]
}

// Set TDS target
{
  "action": "set_tds_target",
  "value": 1.2
}

// Set zone duration
{
  "action": "set_duration",
  "zone": 1,
  "duration": 600
}

// Direct relay override
{
  "action": "relay",
  "channel": 5,
  "state": true
}
```

### Topic: `smartflow/{device_id}/status`
**Direction:** ESP32 → Broker (published on connect, disconnect, fault)  
**QoS:** 1

```json
{
  "device_id": "sf2_001",
  "status": "online",
  "firmware_version": "1.0.0",
  "uptime_seconds": 3600,
  "wifi_rssi": -65,
  "timestamp": "2026-03-25T14:30:00Z"
}
```

---

## Protocol Design Notes

1. **ASCII over binary:** All commands are human-readable. Any serial terminal (PuTTY, screen, minicom) can be used to debug or manually control the system without custom tools.

2. **Newline termination:** The `\n` delimiter allows simple byte-accumulation receive buffers without needing message length headers.

3. **No checksums:** Acceptable for a local UART link (≤10 m, controlled environment). For RS-485 long-distance deployment, add CRC16 to each frame.

4. **One-way Sensor → Relay:** The Relay Module does not acknowledge commands. This simplifies the Relay Module firmware considerably. Reliability is acceptable given the short UART cable distance.

5. **MQTT JSON over raw text:** The ESP32 layer uses structured JSON to allow the mobile app and cloud automation scripts to add new command types without firmware changes on the STM32 side.
