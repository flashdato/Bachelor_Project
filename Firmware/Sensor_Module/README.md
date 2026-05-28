# Sensor Module Firmware — STM32F103C8T6
**Status:** Functional prototype  
**IDE:** STM32CubeIDE v1.13.2  
**HAL version:** STM32Cube_FW_F1_V1.8.x  

---

## File Structure (STM32CubeIDE project)

```
Smart Flow 2.0 Sensor Module/
├── Core/
│   ├── Inc/
│   │   └── main.h
│   └── Src/
│       ├── main.c          ← All application logic
│       ├── stm32f1xx_hal_msp.c
│       └── stm32f1xx_it.c  ← Interrupt handlers (TIM2, USART3)
├── Drivers/
│   └── STM32F1xx_HAL_Driver/
└── Smart Flow 2.0 Sensor Module.ioc  ← CubeMX configuration
```

---

## Peripheral Configuration (.ioc)

| Peripheral | Configuration | Purpose |
|-----------|--------------|---------|
| RCC | HSE 8MHz, PLL ×9 = 72MHz | System clock |
| ADC1 | CH0 (PA0), CH1 (PA1), 12-bit | TDS and pH sensors |
| TIM2 | PSC=7200, ARR=10000, interrupt | 1 Hz sequence ticker |
| USART1 | 9600 8N1, TX=PA9, RX=PA10 | Relay Module commands |
| USART3 | 9600 8N1, TX=PB10, RX=PB11, RX interrupt | ESP32 bridge |

---

## Main Loop — ADC Sampling

```c
while (1) {
    // Switch to TDS channel (PA0)
    sConfig.Channel = ADC_CHANNEL_0;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    adcBuffer_TDS[tdsIndex++ % 30] = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    // Switch to pH channel (PA1)
    sConfig.Channel = ADC_CHANNEL_1;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    adcBuffer_pH[phIndex++ % 30] = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    // Calculate averages and engineering values
    tdsValue   = calculateTDS(average(adcBuffer_TDS, 30));
    pressureVal = calculatePressure(readValue2);
    // phValue = 7.0f;  (hardcoded; real calculation commented out)
}
```

---

## TIM2 ISR — Irrigation State Machine

```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        sequenceTick();  // runs every ~1 second
    }
}

void sequenceTick(void) {
    switch (irrigationSequence) {
        case 0:  break;  // Idle
        case 1:  // Pump ON + first zone open
            sendRelayCmd("101");  // pump ON
            sendRelayCmd("105");  // zone 3 valve ON  ← BUG: always zone3
            irrigationSequence = 2;
            break;
        case 2:  // Wait for zone 1 duration
            if (++zone1Timer >= zone1Duration) irrigationSequence = 3;
            break;
        case 3:  // Fertilizer ON + TDS feedback
            sendRelayCmd("102");  // fertilizer pump ON
            adjustFertilizer();   // compare tdsValue vs tdsTarget → M0 or M1
            irrigationSequence = 4;
            break;
        // ... states 4–13 follow same pattern for remaining zones
        case 13:  // Last zone close + pump OFF
            sendRelayCmd("001");  // pump OFF
            irrigationSequence = 0;
            break;
    }
}
```

---

## USART3 ISR — Command Processing

```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        if (rxByte == '\n') {
            rxBuffer[rxIndex] = '\0';
            processCommand(rxBuffer);
            rxIndex = 0;
        } else {
            if (rxIndex < 99) rxBuffer[rxIndex++] = rxByte;
        }
        HAL_UART_Receive_IT(&huart3, &rxByte, 1);  // re-arm
    }
}

void processCommand(char *cmd) {
    if (strcmp(cmd, "999") == 0) {
        // Send sensor telemetry
        char buf[64];
        sprintf(buf, "TDS:%.2f\npH:%.1f\nP:%.2f\n", tdsValue, phValue, pressureVal);
        HAL_UART_Transmit(&huart3, (uint8_t*)buf, strlen(buf), 100);
    } else if (strcmp(cmd, "801") == 0) {
        irrigationSequence = 1;
    } else if (cmd[0] == 'S') {
        int zone = cmd[1] - '0';
        int duration = atoi(cmd + 2);
        setZoneDuration(zone, duration);
    } else if (cmd[0] == 'E') {
        tdsTarget = atoi(cmd + 1) / 1000.0f;
    } else if (cmd[0] == 'R') {
        int relay = atoi(cmd + 1);
        activateRelay(relay);
    }
}
```

---

## Global Variables

| Variable | Type | Description |
|----------|------|-------------|
| `adcBuffer_TDS[30]` | `uint16_t[]` | Circular ADC buffer for TDS probe |
| `adcBuffer_pH[30]` | `uint16_t[]` | Circular ADC buffer for pH electrode |
| `tdsValue` | `float` | Current TDS reading in ppm |
| `phValue` | `float` | Current pH reading (hardcoded 7.0) |
| `pressureVal` | `float` | Current pressure in bar |
| `irrigationSequence` | `uint8_t` | Current FSM state (0=idle, 1–13=active) |
| `zone1Duration` – `zone4Duration` | `uint16_t` | Zone irrigation durations in seconds |
| `tdsTarget` | `float` | User-set TDS target for fertigation feedback |
| `rxBuffer[100]` | `char[]` | USART3 receive buffer |
| `rxByte` | `uint8_t` | Single-byte USART3 DMA/IT target |

---

## Known Issues

| # | Description | Impact |
|---|-------------|--------|
| 1 | `uint8_t rxData` declared twice (lines 54, 57) | Compiler warning; shadows variable |
| 2 | Nested function definitions inside `main()` | Non-standard GCC extension; may not compile on strict C99 |
| 3 | USART1 `HAL_UART_Receive_IT` called but callback ignores USART1 | Dead receive code |
| 4 | pH hardcoded to 7.0 | pH monitoring non-functional |
| 5 | Pressure: `readValue2 * 5 / 1024` — integer division | Reading loses precision; use `5.0f / 1024.0f` |
| 6 | Zone relay cycling in sequence always uses relay3 | Zones 2–4 never get scheduled in sequence |
| 7 | `tdsValue_User > 1` reset to -1 | Logic is inverted/nonsensical |
