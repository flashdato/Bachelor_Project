# Relay Module Firmware — STM32F103C8T6
**Status:** Partial (relays 3, 4, 9, 10, 11 commented out)  
**IDE:** STM32CubeIDE v1.13.2  
**HAL version:** STM32Cube_FW_F1_V1.8.x  

---

## Design Philosophy

The Relay Module firmware is intentionally simple — **it contains no irrigation logic**. Its entire purpose is to receive 3-character ASCII commands over UART and toggle GPIO outputs accordingly. All intelligence lives in the Sensor Module.

This separation allows:
- The Relay Module to be physically located near the actuators (possibly 10+ meters away)
- The Sensor Module to be upgraded independently without touching Relay Module firmware
- Any device that can send ASCII over UART to control the relays directly

---

## Peripheral Configuration (.ioc)

| Peripheral | Configuration | Purpose |
|-----------|--------------|---------|
| RCC | HSI 8MHz (internal) | System clock — no crystal needed |
| USART3 | 9600 8N1, RX=PB11, TX=PB10, RX interrupt | Receives relay commands |
| GPIO PA0-PA7, PA12, PA15, PB3-PB8 | Output Push-Pull | Relay control signals |

---

## USART3 ISR — Command Decode

```c
uint8_t rxByte;
char rxBuffer[32];
uint8_t rxIndex = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        if (rxByte == '\n') {
            rxBuffer[rxIndex] = '\0';
            parseRelayCommand(rxBuffer);
            rxIndex = 0;
        } else {
            if (rxIndex < 31) rxBuffer[rxIndex++] = rxByte;
        }
        HAL_UART_Receive_IT(&huart3, &rxByte, 1);
    }
}

void parseRelayCommand(char *cmd) {
    // Check stepper commands first (non-numeric prefix)
    if (cmd[0] == 'M') {
        if (cmd[1] == '0') stepperDirection = DECREASE;
        if (cmd[1] == '1') stepperDirection = INCREASE;
        stepperSteps = 0;
        return;
    }

    int code = atoi(cmd);  // parse "101" → 101
    switch (code) {
        case 1:   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET); break;  // Relay1 OFF
        case 101: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);   break;  // Relay1 ON
        case 2:   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); break;  // Relay2 OFF
        case 102: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);   break;  // Relay2 ON
        // case 3/103: PA3 — commented out
        // case 4/104: PA4 — commented out
        case 5:   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET); break;  // Relay5 OFF
        case 105: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);   break;  // Relay5 ON
        case 6:   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); break;  // Relay6 OFF
        case 106: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);   break;  // Relay6 ON
        case 7:   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET); break; // Relay7 OFF
        case 107: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);   break; // Relay7 ON
        case 8:   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); break;  // Relay8 OFF
        case 108: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   break;  // Relay8 ON
        // case 9/109:  PA12 — commented out
        // case 10/110: PA6  — commented out
        // case 11/111: PA7  — commented out
        default: break;
    }
}
```

---

## Main Loop — Stepper Motor

```c
int stepperDirection = NONE;  // set by UART ISR
int stepperSteps = 0;

int main(void) {
    // ... HAL_Init, clock config, peripheral init ...
    HAL_UART_Receive_IT(&huart3, &rxByte, 1);  // arm first receive

    while (1) {
        if (stepperDirection == INCREASE && stepperSteps < 90) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // coil B HIGH
            HAL_Delay(100);                                        // ← BLOCKING
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // coil B LOW
            stepperSteps++;
        } else if (stepperDirection == DECREASE && stepperSteps < 90) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // coil A HIGH
            HAL_Delay(100);                                        // ← BLOCKING
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // coil A LOW
            stepperSteps++;
        } else {
            stepperDirection = NONE;
            stepperSteps = 0;
        }
    }
}
```

**Known issue:** `HAL_Delay(100)` in the main loop blocks for 100ms per step. During a 90-step adjustment (~9 seconds), any incoming relay commands are queued by the UART interrupt but not processed until the stepper loop completes. This is acceptable for the prototype but should be replaced with a timer-driven stepper task in a future revision.

---

## Startup Behavior

On power-on or reset:
- All GPIO outputs initialize to **LOW** (all relays open, pump off, valves closed)
- USART3 receive interrupt armed immediately
- Main loop enters stepper polling — no relay activations until a command arrives

This ensures **fail-safe behavior**: a power cycle or reset leaves all actuators in a safe de-energized state.

---

## Known Issues

| # | Description | Impact |
|---|-------------|--------|
| 1 | Relays 3, 4, 9, 10, 11 handlers commented out | 5 channels non-functional via firmware |
| 2 | Stepper `HAL_Delay(100)` in main loop | Blocks UART processing during long stepper moves |
| 3 | No acknowledgment sent back to Sensor Module | Cannot detect Relay Module failure from Sensor side |
