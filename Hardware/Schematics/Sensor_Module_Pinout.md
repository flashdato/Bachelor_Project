# Sensor Module — Hardware Pinout
**MCU:** STM32F103C8T6 (Blue Pill)  
**Clock:** 72 MHz from external 8 MHz crystal (HSE × 9 PLL)  
**IDE:** STM32CubeIDE v1.13.2  

---

## Pin Assignment Table

| Pin | Peripheral | Direction | Connected To | Notes |
|-----|-----------|-----------|-------------|-------|
| PA0 | ADC1 CH0 | Input (analog) | TDS sensor probe | 30-sample rolling average buffer |
| PA1 | ADC1 CH1 | Input (analog) | pH electrode | High-impedance; use shielded coaxial |
| PA9 | USART1 TX | Output | Relay Module RX (PB11) | 9600 baud, 8N1 — sends relay commands |
| PA10 | USART1 RX | Input | Relay Module TX (PB10) | Currently unused (one-way master) |
| PB10 | USART3 TX | Output | ESP32 UART RX | 9600 baud — sends sensor telemetry |
| PB11 | USART3 RX | Input | ESP32 UART TX | 9600 baud — receives remote commands |

---

## Timer Configuration

| Timer | Prescaler | Auto-Reload | Interrupt Frequency | Purpose |
|-------|-----------|-------------|-------------------|---------|
| TIM2 | 7200 | 10000 | ~1 Hz | Irrigation sequence state machine tick |

Formula: `f = 72,000,000 / ((7200) × (10000)) = 1.0 Hz`

---

## ADC Configuration

- **Peripheral:** ADC1 (single ADC, channel-switched in software)
- **Resolution:** 12-bit (0–4095)
- **Reference:** 3.3V (Vref+)
- **Sampling:** Software-triggered, channels switched manually between readings
- **Filtering:** 30-sample circular buffer, averaged before conversion to engineering units

### TDS Calculation
```
Voltage (V) = (avgADC / 4096.0) × 3.3
TDS (ppm)   = (170.42 × V³ - 253.86 × V² + 850.39 × V) × 0.5 / temperatureCoeff
temperatureCoeff = 1.0 + 0.02 × (temperature - 25.0)   [fixed at 19°C in firmware]
```

### pH Calculation (design — currently hardcoded to 7.0 in firmware)
```
Voltage (V) = (avgADC / 4096.0) × 3.3
pH = 3.5 × Voltage + offset    [two-point calibration: pH 4.01 and pH 6.86 buffers]
```

### Pressure Calculation
```
Voltage (V) = (readValue2 × 5.0 / 1024.0)   [NOTE: integer division bug in current firmware]
Pressure (bar) = (Voltage - 0.5) × (10.0 / 4.0)
```

---

## UART Protocols

### USART1 — Sensor Module to Relay Module
- **Baud rate:** 9600
- **Format:** ASCII, newline-terminated (`\n`)
- **Direction:** Sensor Module is master (transmit only in normal operation)
- **Command format:** 3-digit ASCII code

| Command | Action |
|---------|--------|
| `001\n` | Relay 1 OFF |
| `101\n` | Relay 1 ON |
| `002\n` | Relay 2 OFF |
| `102\n` | Relay 2 ON |
| ... | (see Protocol/README.md for full table) |
| `M0\n` | Stepper motor decrease direction |
| `M1\n` | Stepper motor increase direction |

### USART3 — ESP32 to Sensor Module
- **Baud rate:** 9600
- **Format:** ASCII, newline-terminated (`\n`)
- **Receive buffer:** 100 bytes, accumulated until `\n` received

| Command | Response / Action |
|---------|------------------|
| `999\n` | Reply: `TDS:x.xx\npH:x.x\nP:x.x\n` |
| `801\n` | Start irrigation sequence |
| `S1xxxx\n` | Set Zone 1 duration (seconds) |
| `S2xxxx\n` | Set Zone 2 duration (seconds) |
| `S3xxxx\n` | Set Zone 3 duration (seconds) |
| `S4xxxx\n` | Set Zone 4 duration (seconds) |
| `Exxxx\n` | Set user TDS target (value / 1000) |
| `Rxx\n` | Directly activate zone relay (relay 3–11 index) |

---

## Irrigation Sequence State Machine

The TIM2 1 Hz interrupt drives a 13-state FSM:

| State | Action |
|-------|--------|
| 0 | Idle — monitor sensors, accept commands |
| 1 | Pump ON + Zone 1 valve open |
| 2 | Wait (Zone 1 duration counter) |
| 3 | Fertilizer pump ON (TDS feedback active) |
| 4 | Wait (fertilizer interval) |
| 5 | Zone 1 valve close, Zone 2 valve open |
| 6 | Wait (Zone 2 duration) |
| 7 | Fertilizer pump ON |
| 8 | Wait |
| 9 | Zone 2 close, Zone 3 open |
| 10 | Wait (Zone 3 duration) |
| 11 | Fertilizer pump ON |
| 12 | Wait |
| 13 | Zone 3 close, pump OFF → State 0 |

TDS feedback during fertilizer states: if TDS < target, send `M1\n` (increase fertilizer); if TDS > target, send `M0\n` (decrease).

---

## Notes

- USART1 `Receive_IT` is initialized in firmware but the callback never handles USART1 — dead code.
- pH reading is hardcoded to 7.0 in current firmware; real Nernst-equation calculation is commented out.
- `uint8_t rxData` is declared twice in the current firmware source (known duplicate).
