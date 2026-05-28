# Smart Flow 2.0
## Automated and Remotely Controlled Irrigation System

**Author:** Davit Kavelashvili  
**Type:** Bachelor's Thesis Project  
**Started:** March 7, 2026  
**Total BOM Cost:** ~$70 USD (4-zone installation)

---

## Overview

Smart Flow 2.0 is a modular, low-cost, dual-processor automated irrigation and fertigation system designed for small-to-medium agricultural operations. It addresses the gap between cheap fixed-schedule irrigation timers and expensive commercial smart irrigation platforms costing $500–$2,000+.

The system combines real-time water quality monitoring (TDS, pH, pressure), automated multi-zone valve control, and remote access over WiFi/MQTT — all built from commodity hardware for approximately $70 in components.

---

## Repository Structure

```
Bachelor_Project/
├── Hardware/
│   ├── Schematics/          ← Pinouts, wiring diagrams, system architecture
│   ├── BOM.md               ← Full bill of materials with costs
│   └── Power_System.md      ← Power architecture (solar/grid hybrid)
├── Firmware/
│   ├── Sensor_Module/       ← STM32 Sensor Module firmware documentation
│   ├── Relay_Module/        ← STM32 Relay Module firmware documentation
│   └── ESP32/               ← ESP32 cloud bridge firmware (design + implementation)
├── Protocol/
│   └── README.md            ← Full inter-module communication protocol reference
├── Research_and_Sources/
│   ├── Market_Analysis/     ← Global smart irrigation market reports
│   ├── Industry_Competitors/← Netafim, Rain Bird, Rachio documentation
│   └── Technical_Modules/  ← Datasheets, protocol standards, IDE references
├── Media/                   ← Prototype photos and video demonstrations
└── Manual_Uploads/          ← Periodic Google Docs snapshots
```

---

## System Architecture

Smart Flow 2.0 uses a **dual-STM32 + ESP32** architecture split across three physical modules:

```
[Sensors] ──ADC──► [Sensor Module STM32]  ◄──UART3──► [ESP32] ◄──WiFi──► [MQTT Broker] ◄──► [Mobile App]
                           │
                        UART1 (USART1)
                           │
                           ▼
                   [Relay Module STM32] ──GPIO──► [Relay Board] ──► [Pumps / Valves]
```

| Module | MCU | Clock | Role |
|--------|-----|-------|------|
| Sensor Module | STM32F103C8T6 | 72 MHz (HSE) | Sensor acquisition, irrigation sequencing, command processing |
| Relay Module | STM32F103C8T6 | 8 MHz (HSI) | Relay switching, stepper motor control |
| WiFi Gateway | ESP32-WROOM-32 | 240 MHz | WiFi, MQTT broker bridge, NTP sync |

**Why two STM32s?** Running 11 relay control wires from a central controller to valves spread across a field is impractical. A two-wire UART cable between modules allows the Relay Module to be physically co-located with the actuators while the Sensor Module stays near the sensors and water manifold.

---

## Key Features

- **Multi-zone irrigation** — up to 7 solenoid valve zones (11 relay channels total)
- **Real-time water quality** — TDS (nutrient concentration) and pH monitoring with 30-sample rolling average filter
- **Automated fertigation** — stepper motor-controlled mixing valve with TDS feedback loop
- **Remote control** — WiFi/MQTT with JSON command interface
- **Off-grid capable** — solar panel + battery power architecture
- **Open protocol** — all inter-module communication uses human-readable ASCII over standard UART
- **Autonomous fallback** — all irrigation logic runs locally on STM32; WiFi loss does not interrupt active sequences

---

## Competitive Position

| Product | Price | Zones | TDS/pH Monitoring | Fertigation | Open Protocol |
|---------|-------|-------|-------------------|-------------|---------------|
| Rain Bird IQ4 | ~$2,000+ | Up to 48 | No | No | No |
| Rachio 3 | ~$230 | 8–16 | No | No | No |
| OpenSprinkler | ~$170 | Up to 72 | No | No | Yes |
| **Smart Flow 2.0** | **~$70** | **7** | **Yes** | **Yes** | **Yes** |

---

## Live Documentation

The full thesis document is maintained separately:  
📄 [Google Docs — Live Document](https://docs.google.com/document/d/1qkrIf-M12A-TOWwefRMs7QSAykDUy9xsk56rvhWRvOg/edit?usp=sharing)

Periodic snapshots are archived in `/Manual_Uploads/`.

---

## Quick Links

- [Hardware Pinouts & Schematics](Hardware/Schematics/)
- [Bill of Materials](Hardware/BOM.md)
- [Firmware Overview](Firmware/README.md)
- [Communication Protocol Reference](Protocol/README.md)
- [Sensor Calibration Procedures](Hardware/Schematics/Sensor_Calibration.md)
