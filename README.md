# Smart Flow 2.0
## Automated and Remotely Controlled Irrigation System

**Author:** Davit Kavelashvili  
**Institution:** Kutaisi International University (KIU)  
**Type:** Bachelor's Thesis Project  
**Started:** March 7, 2026  
**Status:** ✅ Thesis submitted | ✅ Hardware tested | 🔄 ESP32 firmware pending

---

## 📄 Download Thesis

| Version | File | Date |
|---------|------|------|
| **Final (submitted)** | [Bachelor_Project.docx](Manual_Uploads/Bachelor_Project.docx) | June 2026 |
| Previous draft | [Bachelor_Project_28.06.2026.docx](Manual_Uploads/Bachelor_Project_28.06.2026.docx) | May 2026 |
| First draft | [Bachelor_Project_07.03.2026.docx](Manual_Uploads/Bachelor_Project_07.03.2026.docx) | March 2026 |

---

## Overview

Smart Flow 2.0 is a modular, low-cost, dual-processor automated irrigation and fertigation system designed for small-to-medium agricultural operations. It addresses the gap between cheap fixed-schedule irrigation timers and expensive commercial smart irrigation platforms costing $500–$2,000+.

The system combines real-time water quality monitoring (TDS, pH), automated multi-zone valve control, and closed-loop fertilizer dosing via a stepper-driven mixing valve — all built from commodity hardware for approximately **$52 in electronics**.

---

## What Works (Tested)

| Subsystem | Status |
|-----------|--------|
| TDS sensor reading (30-sample rolling average) | ✅ Tested — within 5% at <700 ppm |
| 4-zone solenoid valve control | ✅ Tested — 20 activation cycles, no cross-activation |
| 13-state irrigation sequence state machine | ✅ Tested — 3 full cycles completed |
| Stepper motor TDS feedback loop | ✅ Tested — responds to threshold triggers |
| Sensor ↔ Relay UART communication | ✅ Tested — 24-hour continuous run, zero errors |
| pH sensor (ADC + buffer) | ⚠️ Hardware wired, calibration offset not finalized |
| ESP32 cloud bridge | 🔄 Architecture designed, firmware not yet written |
| Mobile app ↔ hardware end-to-end | 🔄 App built; pending ESP32 implementation |

---

## System Architecture

Smart Flow 2.0 uses a **dual-STM32 + ESP32** architecture split across three physical modules:

```
[Sensors] ──ADC──► [Sensor Module STM32]  ◄──UART3──► [ESP32] ◄──WiFi──► [MQTT Broker] ◄──► [Mobile App]
                           │
                        UART1 (USART1)
                           │
                           ▼
                   [Relay Module STM32] ──GPIO──► [Relay Board] ──► [Pumps / Valves / Stepper]
```

| Module | MCU | Clock | Role |
|--------|-----|-------|------|
| Sensor Module | STM32F103C8T6 | 72 MHz (HSE + PLL×9) | Sensor acquisition, irrigation sequencing, command processing |
| Relay Module | STM32F103C8T6 | 8 MHz (HSI) | Relay switching, stepper motor control |
| WiFi Gateway | ESP32-WROOM-32 | 240 MHz dual-core | WiFi, MQTT bridge, NTP time sync |

**Why two STM32s?** Running ADC conversions continuously on the same board that switches relay coils introduces electrical noise on the power and ground planes that corrupts analog readings. Separating the analog sensing side from the relay switching side — and connecting them over a clean serial link — is standard practice in industrial instrumentation. It also means each board does exactly one thing.

---

## Key Features

- **Multi-zone irrigation** — 4 active solenoid valve zones (11 relay channels total, 7 firmware-enabled)
- **Real-time water quality** — TDS (nutrient concentration) monitoring with 30-sample rolling average filter
- **Closed-loop fertigation** — stepper motor-controlled mixing valve adjusts fertilizer concentration automatically based on live TDS readings
- **Remote control** — WiFi/MQTT with JSON command interface (designed; implementation pending)
- **Autonomous fallback** — all irrigation logic runs locally on STM32; WiFi loss does not interrupt active sequences
- **Open protocol** — all inter-module communication uses human-readable ASCII over standard UART; debuggable with any serial terminal at 9600 baud
- **Fail-safe power-on** — all GPIO outputs initialize LOW; pump and valves are de-energized on reset or power loss

---

## Competitive Position

| Product | Price | Zones | TDS/pH Monitoring | Closed-loop Fertigation |
|---------|-------|-------|-------------------|------------------------|
| Rain Bird IQ4 | ~$2,000+ | Up to 48 | No | No |
| Rachio 3 | ~$230 | 8–16 | No | No |
| OpenSprinkler | ~$170 | Up to 72 | No | No |
| **Smart Flow 2.0** | **~$52** | **4–7** | **Yes (TDS active, pH HW-ready)** | **Yes** |

> No product under $200 from any established manufacturer combines TDS monitoring with closed-loop fertigation and remote WiFi control. Smart Flow 2.0 is the only system on this list that does all three.

---

## Component Cost (Electronics Only)

| Component | Qty | Unit | Total |
|-----------|-----|------|-------|
| STM32F103C8T6 "Blue Pill" | 2 | $2.00 | $4.00 |
| ESP32-WROOM-32 | 1 | $4.00 | $4.00 |
| 12-channel relay board (5V) | 1 | $6.00 | $6.00 |
| DFRobot TDS sensor (SEN0244) | 1 | $7.00 | $7.00 |
| Analog pH sensor (SEN0161) | 1 | $9.00 | $9.00 |
| LM2596 buck converter | 1 | $1.50 | $1.50 |
| 28BYJ-48 stepper + ULN2003 | 1 | $2.50 | $2.50 |
| 12V solenoid valves (¾" NC) | 4 | $3.50 | $14.00 |
| 12V water pump | 1 | $4.00 | $4.00 |
| **Total** | | | **$52.00 (~143 GEL)** |

Pipe, fittings, and wiring are excluded — these vary with the size of the irrigated area and are treated as site infrastructure, not controller cost.

---

## Repository Structure

```
Bachelor_Project/
├── Hardware/
│   ├── Schematics/
│   │   ├── System_Architecture.md     ← 4-layer system model, data flow, power architecture
│   │   ├── Sensor_Module_Pinout.md    ← Pin assignments, ADC config, UART protocol tables
│   │   └── Relay_Module_Pinout.md     ← Pin-to-relay mapping, stepper motor control
│   ├── BOM.md                         ← Full bill of materials with costs and sourcing
│   └── Power_System.md                ← Power architecture, battery sizing, wiring specs
├── Firmware/
│   ├── README.md                      ← Firmware overview, dev environment, known issues
│   ├── Sensor_Module/
│   │   ├── README.md                  ← Architecture, ADC pipeline, FSM, ISR logic
│   │   └── Source_Code/main.c         ← Full STM32 Sensor Module firmware
│   ├── Relay_Module/
│   │   ├── README.md                  ← Command decoder, stepper logic, startup behavior
│   │   └── Source_Code/main.c         ← Full STM32 Relay Module firmware
│   └── ESP32/
│       └── README.md                  ← FreeRTOS 3-task architecture design (not yet implemented)
├── Protocol/
│   └── README.md                      ← Complete inter-module communication reference
├── Research_and_Sources/
│   ├── Market_Analysis/Links.txt      ← Smart irrigation market reports and data sources
│   ├── Industry_Competitors/Links.txt ← Competitor product documentation
│   └── Technical_Modules/Links.txt    ← Component datasheets, protocol specs, IDE references
├── Media/                             ← Prototype photos and video demonstrations
└── Manual_Uploads/                    ← Thesis document versions
    └── README.md
```

---

## Future Work

- [ ] Implement ESP32 firmware (WiFi Manager + MQTT Bridge + STM32 Serial Task — architecture in `Firmware/ESP32/README.md`)
- [ ] Complete pH probe calibration (two-point: pH 4.01 and pH 6.86 buffers)
- [ ] Wire pressure sensor readings into irrigation control logic (currently calculated but unused)
- [ ] Field deployment and crop validation (bench-tested only)
- [ ] iOS support for the mobile application (Android only currently)

---

## Quick Links

- [📥 Download Final Thesis](Manual_Uploads/Bachelor_Project.docx)
- [Hardware Pinouts](Hardware/Schematics/)
- [Bill of Materials](Hardware/BOM.md)
- [Firmware Overview](Firmware/README.md)
- [Communication Protocol Reference](Protocol/README.md)
- [Power System Design](Hardware/Power_System.md)
