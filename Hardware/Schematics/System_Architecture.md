# System Architecture — Smart Flow 2.0
**Verification Date:** March 25, 2026

---

## 1. Four-Layer System Model

Smart Flow 2.0 is organized into four logical layers:

```
┌─────────────────────────────────────────────────┐
│  Layer 4 — User Interface (Mobile App)          │
│  Dashboard / Zone Control / Schedule / Logs     │
└───────────────────┬─────────────────────────────┘
                    │ MQTT over TLS (WiFi)
┌───────────────────▼─────────────────────────────┐
│  Layer 3 — Cloud & Connectivity (ESP32)         │
│  MQTT Broker Bridge / NTP / WiFi Reconnect      │
└───────────────────┬─────────────────────────────┘
                    │ UART3 @ 9600 baud (ASCII)
┌───────────────────▼─────────────────────────────┐
│  Layer 2 — Sensing & Scheduling (Sensor Module) │
│  TDS/pH/Pressure ADC · 13-State FSM · UART Cmd  │
└───────────────────┬─────────────────────────────┘
                    │ UART1 @ 9600 baud (3-digit ASCII)
┌───────────────────▼─────────────────────────────┐
│  Layer 1 — Actuation (Relay Module)             │
│  11 Relay Channels · Stepper Motor Driver       │
└─────────────────────────────────────────────────┘
```

---

## 2. Data Flow Paths

### Control Path (Downward)
```
User (Mobile App)
  │ publishes: smartflow/{id}/cmd  {"action": "start_sequence", ...}
  ▼
MQTT Broker (HiveMQ Cloud)
  │ subscribes: smartflow/{id}/cmd
  ▼
ESP32 Gateway
  │ translates JSON → ASCII command → "801\n"
  │ UART3 TX (GPIO17)
  ▼
Sensor Module STM32
  │ interprets command, manages irrigation state machine
  │ USART1 TX (PA9) → "101\n" (relay ON command)
  ▼
Relay Module STM32
  │ sets GPIO output HIGH
  ▼
Relay Board → Solenoid Valve / Pump OPEN
```

### Telemetry Path (Upward)
```
Sensors (TDS probe PA0, pH electrode PA1)
  │ ADC1, 30-sample rolling average
  ▼
Sensor Module STM32
  │ formats: "TDS:1.24\npH:6.8\nP:1.5\n"
  │ USART3 TX (PB10)
  ▼
ESP32 Gateway
  │ publishes: smartflow/{id}/sensors
  │ {"tds": 1.24, "ph": 6.8, "pressure": 1.5, ...}
  ▼
MQTT Broker
  │ subscribes: smartflow/{id}/sensors
  ▼
Mobile App — live readings on Dashboard screen
```

---

## 3. Power Architecture

```
                    ┌──────────────────┐
[ SOLAR PANEL ] ───►│ CHARGE CONTROLLER│◄──── [ 12V DC ADAPTER ]
                    │   (backup grid)  │
                    └────────┬─────────┘
                             │
                    ┌────────▼─────────┐
                    │  BATTERY STORAGE │
                    │  (12V lead-acid  │
                    │   or LiFePO4)    │
                    └────────┬─────────┘
                             │ 12V rail
              ┌──────────────┼──────────────┐
              │              │              │
    ┌─────────▼──────┐  ┌────▼────┐  ┌─────▼──────────────┐
    │ LM2596 Buck    │  │ LM2596  │  │  HIGH-POWER BUS    │
    │ 12V → 5V       │  │ 12V→3.3V│  │  12V actuators     │
    │ (ESP32, relays)│  │ (STM32s)│  │  (pump, valves)    │
    └────────────────┘  └─────────┘  └────────────────────┘
```

**Power budget (active irrigation mode):**
| Component | Current Draw |
|-----------|-------------|
| Sensor Module STM32 | ~50 mA |
| Relay Module STM32 | ~50 mA |
| ESP32 (WiFi active) | ~150–200 mA |
| 8-channel relay board | ~80 mA (coils energized) |
| Solenoid valve (1 open) | ~300–500 mA at 12V |
| **Total** | **~400–600 mA @ 12V** |

---

## 4. Component Roles

| Component | Role | Failure Mode |
|-----------|------|-------------|
| Sensor Module STM32 | Local intelligence; runs independently of WiFi | Power loss: GPIO → LOW (all relays OFF — safe) |
| Relay Module STM32 | Pure actuator; no logic | UART loss: holds last state until power cycle |
| ESP32 | Remote bridge only | Disconnection: STM32 continues active sequence uninterrupted |
| MQTT Broker | Message routing | Outage: ESP32 reconnects with exponential backoff |
| Relay boards | Galvanic isolation (optocoupler) | Protects logic electronics from valve voltage spikes |

---

## 5. Physical Wiring Summary

| Connection | Cable Type | Max Distance |
|------------|-----------|-------------|
| Sensor Module ↔ Relay Module (UART) | 2-conductor shielded | ~10 m (UART limit; extend with RS-485 adapter) |
| TDS/pH probes ↔ Sensor Module | Shielded coaxial | ~3 m (high-impedance analog) |
| Relay Module ↔ Solenoid valves | Standard 2-conductor | ~50 m |
| Relay Module ↔ Main pump | Standard 2-conductor | ~20 m |

All external connectors: IP67-rated waterproof (M12 or equivalent).
