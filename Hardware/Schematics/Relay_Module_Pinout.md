# Relay Module — Hardware Pinout
**MCU:** STM32F103C8T6 (Blue Pill)  
**Clock:** 8 MHz internal RC oscillator (HSI)  
**IDE:** STM32CubeIDE v1.13.2  

---

## Pin Assignment Table

| Pin | Direction | Relay # | Connected To | Notes |
|-----|-----------|---------|-------------|-------|
| PA2 | Output | Relay 1 | Main circulation pump (12V DC, 300W) | Active HIGH |
| PB5 | Output | Relay 2 | Fertilizer dosing pump (12V DC, 5W) | Active HIGH |
| PA3 | Output | Relay 3 | Zone 1 solenoid valve | **Commented out in current firmware** |
| PA4 | Output | Relay 4 | Zone 2 solenoid valve | **Commented out in current firmware** |
| PB4 | Output | Relay 5 | Zone 3 solenoid valve | Active HIGH |
| PB3 | Output | Relay 6 | Zone 4 solenoid valve | Active HIGH |
| PA15 | Output | Relay 7 | Zone 5 solenoid valve | Active HIGH |
| PA5 | Output | Relay 8 | Zone 6 solenoid valve | Active HIGH |
| PA12 | Output | Relay 9 | Zone 7 solenoid valve | **Commented out in current firmware** |
| PA6 | Output | Relay 10 | Auxiliary (expandable) | **Commented out in current firmware** |
| PA7 | Output | Relay 11 | Auxiliary (expandable) | **Commented out in current firmware** |
| PA0 | Output | Stepper A | Fertilizer mixing valve coil A | Decrease-concentration direction |
| PA1 | Output | Stepper B | Fertilizer mixing valve coil B | Increase-concentration direction |
| PB10 | USART3 TX | — | Sensor Module RX (PA10) | Unused — one-way receive only |
| PB11 | USART3 RX | — | Sensor Module TX (PA9) | Receives all relay commands |

---

## UART Configuration

- **Peripheral:** USART3
- **Baud rate:** 9600
- **Format:** 8N1, ASCII, newline-terminated (`\n`)
- **Mode:** Receive-only (interrupt-driven)
- **Buffer:** Bytes accumulated until `\n` received, then command parsed

---

## Command Decoding Logic

The firmware receives a string, parses the first 3 characters as a decimal integer, and switches on the value:

| Received | Action |
|----------|--------|
| `001\n` | PA2 LOW (Relay 1 OFF — pump stops) |
| `101\n` | PA2 HIGH (Relay 1 ON — pump starts) |
| `002\n` | PB5 LOW (Relay 2 OFF — fertilizer pump stops) |
| `102\n` | PB5 HIGH (Relay 2 ON — fertilizer pump starts) |
| `003\n` | PA3 LOW — **(commented out)** |
| `103\n` | PA3 HIGH — **(commented out)** |
| `004\n` | PA4 LOW — **(commented out)** |
| `104\n` | PA4 HIGH — **(commented out)** |
| `005\n` | PB4 LOW (Relay 5 OFF) |
| `105\n` | PB4 HIGH (Relay 5 ON) |
| `006\n` | PB3 LOW (Relay 6 OFF) |
| `106\n` | PB3 HIGH (Relay 6 ON) |
| `007\n` | PA15 LOW (Relay 7 OFF) |
| `107\n` | PA15 HIGH (Relay 7 ON) |
| `008\n` | PA5 LOW (Relay 8 OFF) |
| `108\n` | PA5 HIGH (Relay 8 ON) |
| `009\n` | PA12 LOW — **(commented out)** |
| `109\n` | PA12 HIGH — **(commented out)** |
| `010\n` | PA6 LOW — **(commented out)** |
| `110\n` | PA6 HIGH — **(commented out)** |
| `011\n` | PA7 LOW — **(commented out)** |
| `111\n` | PA7 HIGH — **(commented out)** |
| `M0\n` | Stepper: PA0 pulse sequence (decrease concentration, 0–90 steps) |
| `M1\n` | Stepper: PA1 pulse sequence (increase concentration, 0–90 steps) |

---

## Relay Hardware

- **Relay board type:** Optocoupler-isolated 8-channel relay module (active HIGH trigger)
- **Additional discrete relay modules** for channels 9–11
- **Load capacity:** Each channel rated for 10A @ 250V AC / 10A @ 30V DC
- **Isolation:** Optocoupler on each channel — logic electronics galvanically isolated from 12V actuator bus

---

## Stepper Motor

- **Type:** 2-wire simplified drive (unipolar or bipolar with H-bridge)
- **Use:** Positions fertilizer concentration mixing valve (0 to 90 steps full range)
- **Control:** `HAL_Delay(100)` used in main loop between pulses — **blocking call, risky under rapid command bursts**
- **Coil A (PA0):** Energized for decrease-concentration direction
- **Coil B (PA1):** Energized for increase-concentration direction

---

## Notes

- Relay Module uses 8 MHz HSI (internal oscillator) — lower power, simpler config, no crystal needed
- Relays 3, 4, 9, 10, 11 are wired but their firmware handling is commented out in current code
- The stepper motor blocking delay (`HAL_Delay`) can cause UART commands to queue up if fertilizer adjustments happen during rapid sequential commands
