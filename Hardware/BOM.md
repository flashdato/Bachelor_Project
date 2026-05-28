# Bill of Materials — Smart Flow 2.0
**Configuration:** 4-zone installation  
**Prices:** USD at current market rates (2026)

---

## Core Electronics

| Component | Qty | Unit Price | Total | Notes |
|-----------|-----|-----------|-------|-------|
| STM32F103C8T6 "Blue Pill" board | 2 | $2.00 | $4.00 | Sensor Module + Relay Module |
| ESP32-WROOM-32 development board | 1 | $5.00 | $5.00 | WiFi/MQTT gateway |
| 8-channel optocoupler relay board | 1 | $6.00 | $6.00 | Relay channels 1–8 |
| Single-channel relay modules | 3 | $1.50 | $4.50 | Relay channels 9–11 (optional) |
| LM2596 buck converter modules | 2 | $1.50 | $3.00 | 12V→5V and 12V→3.3V |

---

## Sensors

| Component | Qty | Unit Price | Total | Notes |
|-----------|-----|-----------|-------|-------|
| TDS/EC probe with signal board | 1 | $4.00 | $4.00 | Analog 0–3.3V output |
| pH electrode with signal board | 1 | $8.00 | $8.00 | Analog, requires op-amp buffer board |
| Pressure transducer 0–10 bar | 1 | $5.00 | $5.00 | 0.5–4.5V analog output |

---

## Actuators

| Component | Qty | Unit Price | Total | Notes |
|-----------|-----|-----------|-------|-------|
| 12V DC solenoid valve (¾" BSP) | 4 | $6.00 | $24.00 | Zone valves (sold separately) |
| Stepper motor (28BYJ-48) | 1 | $3.00 | $3.00 | Fertilizer mixing valve actuator |
| ULN2003 stepper driver board | 1 | $1.00 | $1.00 | Included with most 28BYJ-48 kits |

---

## Power System

| Component | Qty | Unit Price | Total | Notes |
|-----------|-----|-----------|-------|-------|
| 12V DC power adapter (3A) | 1 | $7.00 | $7.00 | Grid-connected mode |
| Solar charge controller (10A) | 1 | $8.00 | $8.00 | Optional: off-grid mode |
| 12V 7Ah SLA battery | 1 | $12.00 | $12.00 | Optional: off-grid mode |

---

## Enclosure & Wiring

| Component | Qty | Unit Price | Total | Notes |
|-----------|-----|-----------|-------|-------|
| IP65 plastic enclosure | 1 | $5.00 | $5.00 | Houses all electronics |
| DIN rail (35mm, 30cm) | 1 | $2.00 | $2.00 | Mounts boards inside enclosure |
| IP67 M12 waterproof connectors | 6 | $1.00 | $6.00 | Sensor probe cables |
| 2-conductor shielded cable (3m) | 1 | $3.00 | $3.00 | Sensor Module ↔ Relay Module UART |
| Terminal blocks (20-position strip) | 1 | $2.00 | $2.00 | Valve/pump wiring termination |

---

## Cost Summary

| Category | Subtotal |
|----------|----------|
| Core Electronics | $22.50 |
| Sensors | $17.00 |
| Actuators (excl. solenoid valves) | $4.00 |
| Solenoid Valves (4× zone) | $24.00 |
| Power System (grid mode only) | $7.00 |
| Enclosure & Wiring | $18.00 |
| **Total (grid-connected, 4-zone)** | **~$92.50** |
| **Total (excl. solenoid valves)** | **~$68.50** |

> Solenoid valves are treated as infrastructure cost — they are typically purchased per-zone and re-used when upgrading the controller.

---

## Comparison with Commercial Alternatives

| System | Price | Zones | TDS/pH | Fertigation |
|--------|-------|-------|--------|-------------|
| Rain Bird IQ4 Central Control | ~$2,000+ | Up to 48 | No | No |
| Netafim NetBeat Platform | ~$800–$1,500 | Variable | No | No |
| Rachio 3 (16-zone) | ~$230 | 16 | No | No |
| Hunter HC-1200C | ~$350 | 12 | No | No |
| OpenSprinkler (8-zone) | ~$170 | 8 | No | No |
| **Smart Flow 2.0** | **~$70–93** | **7** | **Yes** | **Yes** |

---

## ROI Analysis (Georgian Small Farm, 2 ha)

| Benefit Source | Annual Saving |
|----------------|--------------|
| Water savings (~20% reduction via sensor-driven scheduling) | ~$20 |
| Fertilizer efficiency (reduced over/under-application) | ~$72 |
| Labor savings (eliminate manual inspection trips) | ~$440 |
| **Total first-year benefit** | **~$532** |
| **ROI on $70 hardware cost** | **~760%** |

Based on: Shock & Wang (2011), FAO Irrigation and Drainage Paper No. 66, Georgian farm labor rates 2023.
