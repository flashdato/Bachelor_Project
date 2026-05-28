# Firmware Overview — Smart Flow 2.0

**IDE:** STM32CubeIDE v1.13.2 (STM32 modules) / Arduino IDE or ESP-IDF (ESP32)  
**HAL:** STMicroelectronics HAL (Hardware Abstraction Layer) — UM1850  

---

## Module Summary

| Module | Language | Status | Location |
|--------|----------|--------|----------|
| Sensor Module (STM32) | C (HAL) | Complete (functional prototype) | `Firmware/Sensor_Module/` |
| Relay Module (STM32) | C (HAL) | Partial (relays 3,4,9,10,11 commented out) | `Firmware/Relay_Module/` |
| ESP32 Cloud Bridge | C++ (Arduino) | **Design phase — not yet implemented** | `Firmware/ESP32/` |

---

## Sensor Module Firmware

### Three Concurrent Execution Contexts

```
┌─────────────────────────────────────────────────┐
│  MAIN LOOP (continuous)                         │
│  • ADC channel switch (PA0 / PA1)               │
│  • 30-sample circular buffer fill               │
│  • TDS cubic polynomial calculation             │
│  • pH calculation (hardcoded 7.0 currently)     │
│  • Pressure linear calculation                  │
└────────────────────┬────────────────────────────┘
                     │ (shared variables)
┌────────────────────▼────────────────────────────┐
│  TIM2 INTERRUPT @ ~1 Hz                         │
│  • Irrigation sequence state machine (13 states)│
│  • Zone duration countdown timers               │
│  • TDS feedback → stepper commands              │
│  • Relay ON/OFF commands → USART1               │
└────────────────────┬────────────────────────────┘
                     │ (shared rx_buffer)
┌────────────────────▼────────────────────────────┐
│  USART3 RECEIVE INTERRUPT                        │
│  • Byte accumulation until '\n'                 │
│  • Command dispatch:                            │
│    999 → send sensor telemetry                  │
│    801 → start sequence                         │
│    S1-S4 → set zone durations                   │
│    E → set TDS target                           │
│    R → direct relay activate                    │
└─────────────────────────────────────────────────┘
```

### TDS Calculation Pipeline
```c
// 1. Collect 30 ADC samples into circular buffer
adcBuffer[bufferIndex++ % 30] = HAL_ADC_GetValue(&hadc1);

// 2. Average the buffer
float avgADC = sum(adcBuffer) / 30.0f;

// 3. Convert to voltage
float voltage = avgADC * 3.3f / 4096.0f;

// 4. Temperature-compensated cubic polynomial
float tempCoeff = 1.0f + 0.02f * (temperature - 25.0f);  // fixed at 19°C
float tdsValue = (170.42f * pow(voltage,3) - 253.86f * pow(voltage,2) + 850.39f * voltage) * 0.5f / tempCoeff;
```

---

## Relay Module Firmware

### Single Execution Context

The Relay Module has no local logic — it is a pure command executor:

```
┌──────────────────────────────────────────────────┐
│  USART3 RECEIVE INTERRUPT                         │
│  • Bytes → rx_buffer until '\n'                  │
│  • Parse first 3 chars as integer                │
│  • Switch statement: set GPIO HIGH or LOW        │
│  • Special: "M0"/"M1" → set stepper direction flag│
└──────────────────────────────────────────────────┘
                     │ (stepper_direction flag)
┌────────────────────▼─────────────────────────────┐
│  MAIN LOOP                                        │
│  • If stepper_direction flag set:                 │
│    HAL_GPIO_WritePin(PA0/PA1, HIGH)               │
│    HAL_Delay(100)  ← blocking                    │
│    HAL_GPIO_WritePin(PA0/PA1, LOW)                │
│    step_count++                                   │
│  • Clear flag after 90 steps                     │
└──────────────────────────────────────────────────┘
```

---

## ESP32 Firmware (Design)

### Three FreeRTOS Tasks

```
┌────────────────────────────────────────────────┐
│  Task 1: WiFi Manager                          │
│  • Connect on startup (stored credentials)     │
│  • NTP time sync after connect                 │
│  • Exponential backoff reconnection loop       │
└───────────────────┬────────────────────────────┘
                    │
┌───────────────────▼────────────────────────────┐
│  Task 2: MQTT Bridge                           │
│  • Connect to broker (HiveMQ Cloud / custom)   │
│  • Publish to smartflow/{id}/sensors every 10s │
│  • Subscribe to smartflow/{id}/cmd             │
│  • Parse JSON → translate to STM32 ASCII cmd   │
│  • Publish to smartflow/{id}/status on events  │
└───────────────────┬────────────────────────────┘
                    │ UART3 (GPIO17 TX / GPIO16 RX)
┌───────────────────▼────────────────────────────┐
│  Task 3: STM32 Serial Bridge                   │
│  • Forward MQTT commands → UART TX             │
│  • Poll STM32 for sensor data (cmd "999\n")    │
│  • Parse STM32 response → JSON payload         │
└────────────────────────────────────────────────┘
```

### MQTT → STM32 Translation Examples
```
MQTT cmd: {"action": "start_sequence"}
STM32 cmd: "801\n"

MQTT cmd: {"action": "set_duration", "zone": 2, "duration": 900}
STM32 cmd: "S2900\n"

MQTT cmd: {"action": "set_tds_target", "value": 1.2}
STM32 cmd: "E1200\n"

MQTT cmd: {"action": "relay", "channel": 5, "state": true}
STM32 cmd: "105\n"
```

---

## Development Environment Setup

### STM32 (Sensor & Relay Modules)
1. Install [STM32CubeIDE v1.13.2](https://www.st.com/en/development-tools/stm32cubeide.html)
2. Flash via ST-Link V2 programmer (USB)
3. Monitor UART output with any serial terminal @ 9600 baud

### ESP32
1. Install Arduino IDE 2.x with ESP32 board package (Espressif v2.x+)
2. Required libraries: `PubSubClient` (MQTT), `ArduinoJson`, `WiFi` (built-in)
3. Flash via USB-C on ESP32 dev board

---

## Known Issues in Current Firmware

| Module | Issue | Location |
|--------|-------|----------|
| Sensor | `uint8_t rxData` declared twice | Lines 54 & 57 in main.c |
| Sensor | Nested function definitions inside `main()` | Non-standard GCC extension |
| Sensor | USART1 `Receive_IT` started but callback never handles it | Dead code |
| Sensor | pH hardcoded to 7.0 | Real Nernst equation commented out |
| Sensor | Pressure formula integer division: `readValue2 * 5 / 1024` | Loses precision; use float |
| Sensor | Zone relay selection in sequence always picks relay3 first | Broken cycling logic |
| Relay | Relays 3, 4, 9, 10, 11 handlers commented out | Incomplete |
| Relay | Stepper motor uses `HAL_Delay(100)` in main loop | Blocking; risks dropped UART cmds |
