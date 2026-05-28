# Sensor Calibration Procedures

---

## TDS Sensor Calibration

Recalibrate every 3 months or after probe replacement/exposure to strong chemicals.

**Required materials:**
- Deionized or distilled water (0 ppm reference)
- 500 ppm NaCl calibration solution (2.922 g NaCl per 10 L distilled water)
- 1000 ppm NaCl calibration solution
- Temperature measurement (thermometer)
- Serial terminal connected to Sensor Module UART

**Procedure:**
1. Immerse probe in deionized water; record average ADC reading (target: near 0, typically ADC < 50)
2. Immerse probe in 500 ppm solution at known temperature; record ADC reading
3. Immerse probe in 1000 ppm solution; record ADC reading
4. Fit the cubic polynomial to the three data points, or adjust the firmware constants:
   ```c
   tdsValue = (170.42 * voltage * voltage * voltage
              - 253.86 * voltage * voltage
              + 850.39 * voltage) * 0.5 / temperatureCoefficient;
   ```
5. Verify: 500 ppm solution should read 490–510 ppm after calibration

**Temperature compensation:**
```c
temperatureCoefficient = 1.0 + 0.02 * (temperature - 25.0);
```
Current firmware uses a **fixed temperature of 19°C**. For better accuracy, connect a DS18B20 temperature sensor and feed the live reading into this formula.

---

## pH Sensor Calibration

pH electrodes use two-point calibration. Recalibrate monthly or when readings drift by more than 0.2 pH units.

**Required materials:**
- pH 4.01 buffer solution (commercial sachet recommended)
- pH 6.86 buffer solution (commercial sachet recommended)
- Storage solution: 3.0 M KCl (for electrode preservation between uses)
- Distilled water rinse bottle

**Procedure:**
1. Remove electrode from storage solution; rinse with distilled water; gently blot dry (do not rub)
2. Immerse in pH 6.86 buffer; allow 60 seconds to stabilize; record ADC reading → `adc_high`
3. Rinse electrode; immerse in pH 4.01 buffer; wait 60 seconds; record ADC reading → `adc_low`
4. Calculate slope and offset for the linear calibration formula:
   ```
   pH = m × Voltage + b
   where:
     m = (6.86 - 4.01) / (V_high - V_low)
     b = 6.86 - m × V_high
   ```
5. Update firmware constants accordingly

**Important notes:**
- Electrodes must be stored in 3.0 M KCl — **never in distilled water** (destroys the membrane potential)
- Allow 30+ minutes warm-up time for accurate readings after cold storage
- Replace electrode if response time exceeds 2 minutes or slope is less than 50 mV/pH unit

---

## Pressure Sensor Characterization

**Sensor spec:** 0–10 bar range, 0.5–4.5V analog output, linear transfer function

**Transfer function:**
```
Pressure (bar) = (Voltage - 0.5) × (10.0 / 4.0)
Voltage (V)    = ADC_reading × 5.0 / 1024.0    [NOTE: use float arithmetic to avoid integer division]
```

**Two-point verification:**
1. At atmospheric pressure (0 bar gauge): sensor output = 0.5V → ADC ≈ 102 on 10-bit 5V reference
2. At 2.0 bar (known regulated source): sensor output = 1.3V → ADC ≈ 266

**Typical operating range for irrigation system:**
- Minimum working pressure: 0.5 bar (pump running)
- Normal operating pressure: 1.5–2.5 bar
- High pressure alarm threshold: 4.0 bar (check for blocked valves)
- Low pressure alarm threshold: 0.3 bar (pump failure / pipe burst)

---

## Calibration Log Template

| Date | Sensor | Reference | Measured | Error | Adjusted? | Technician |
|------|--------|-----------|----------|-------|-----------|------------|
| | TDS | 500 ppm | | | | |
| | pH | 6.86 | | | | |
| | Pressure | 0 bar | | | | |
