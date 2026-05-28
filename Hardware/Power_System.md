# Power System Design — Smart Flow 2.0

---

## Overview

The power architecture supports both **grid-connected** and **off-grid (solar)** operation without any hardware changes. The system switches between a 12V DC adapter and a solar-charged battery depending on which source is available.

---

## Architecture Diagram

```
[ SOLAR PANEL 20W ] ─────────────────────────────────────┐
                                                          ▼
[ 12V/3A DC ADAPTER ] ──────────────► [ SOLAR CHARGE CONTROLLER / DIODE ]
(grid-connected mode)                           │
                                                │ 12V regulated
                                                ▼
                                       [ 12V BATTERY ]
                                       (7Ah SLA or LiFePO4)
                                                │
                        ┌───────────────────────┼───────────────────────┐
                        │                       │                       │
              ┌─────────▼──────┐      ┌─────────▼──────┐     ┌─────────▼──────┐
              │ LM2596 Buck    │      │ LM2596 Buck    │     │  12V HIGH-POWER │
              │ 12V → 5V       │      │ 12V → 3.3V     │     │  ACTUATOR BUS  │
              │ (ESP32, relay  │      │ (both STM32s)  │     │ (pump, valves) │
              │  board logic)  │      │                │     │                │
              └────────────────┘      └────────────────┘     └────────────────┘
```

---

## Voltage Rails

| Rail | Regulator | Consumers | Notes |
|------|-----------|-----------|-------|
| 12V main | Battery / adapter | Pump relay, solenoid valves, step-down inputs | High-current bus |
| 5V | LM2596 buck | ESP32-WROOM-32, relay board optocoupler input | LM2596: up to 3A output |
| 3.3V | LM2596 buck (or AMS1117 on Blue Pill) | Both STM32F103 boards | STM32 has onboard 3.3V regulator |

---

## Current Budget

| Mode | Component | Current @ 12V |
|------|-----------|--------------|
| Standby (no irrigation) | 2× STM32 | 100 mA |
| | ESP32 (WiFi connected, idle) | 80 mA |
| | Relay board (no relays energized) | 10 mA |
| | **Standby total** | **~190 mA** |
| Active irrigation | + Main pump relay coil | +50 mA |
| | + 1 solenoid valve relay coil | +50 mA |
| | + ESP32 WiFi active transmission | +150 mA |
| | **Active irrigation total** | **~440 mA** |
| Peak (multiple valves) | + 2 additional solenoid relay coils | +100 mA |
| | **Peak total** | **~540 mA** |

> Note: This is relay coil current only. The pump and valve loads themselves are powered directly from the 12V bus through the relay contacts and are not included above.

---

## Battery Sizing

For a 7Ah 12V SLA battery and 440 mA average draw:
```
Theoretical runtime = 7 Ah / 0.44 A = ~15.9 hours
Derated runtime (SLA 50% DoD) = ~8 hours without solar
```

A 20W solar panel in Georgian conditions (~4–5 peak sun hours/day):
```
Daily generation = 20W × 4.5h × 0.8 (efficiency) = 72 Wh = 6 Ah @ 12V
Daily consumption (8h active, 16h standby) = 0.44×8 + 0.19×16 = ~6.6 Ah
```

Net balance is approximately neutral — a 20W panel is the minimum for sustainable off-grid operation. A 40W panel recommended for reliable autonomy.

---

## Wiring Recommendations

- **Main power wiring (12V bus):** minimum 18 AWG for runs up to 3 m; 16 AWG for longer runs
- **Solenoid valve wiring:** 18 AWG, rated for outdoor UV exposure
- **Logic wiring (3.3V/5V inter-module):** shielded 24 AWG; keep away from 12V power cables
- **Fusing:** Install a 2A fast-blow fuse on the 5V rail and a 500 mA fuse on the 3.3V rail; 10A fuse on the 12V main bus
- **Connectors:** IP67-rated on all external penetrations; IP65 minimum on enclosure

---

## Failure Modes and Safety

| Failure | System Behavior |
|---------|----------------|
| Power loss during irrigation | All GPIO → LOW on startup; all relays release → pump and valves close (fail-safe) |
| Battery low voltage | Charge controller disconnects at ~10.8V; STM32 brownout reset; relays release |
| Overcurrent on 12V bus | Manual fuse blows; replace fuse after diagnosing cause |
| Buck converter failure | Affected rail powers off; STM32 or ESP32 resets; does not affect 12V actuator bus |
