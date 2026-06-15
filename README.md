# HX711 Low-Noise Calibration Framework

Design and optimization of a low-noise signal conditioning and calibration framework for HX711-based digital weighing scales.

## Overview

This project documents a digital weighing scale framework built around the HX711 24-bit analog-to-digital converter (ADC). It targets common sources of inaccuracy in low-resource weighing systems, including electrical noise, power-supply fluctuations, calibration error, and temperature drift.

The proposed design combines:

- A second-order RC low-pass filter for hardware-level noise suppression.
- A discrete-time Kalman filter running on an ATmega328P microcontroller.
- Multi-point linear regression calibration for mapping raw ADC counts to mass.
- First-order temperature compensation using a DS18B20 digital temperature sensor.
- Duty-cycled power management for longer battery operation.

## Key Results

| Metric | Result |
| --- | --- |
| Noise reduction | 94% reduction relative to the unfiltered baseline |
| Signal-to-noise ratio | Approximately 12.4 dB improvement |
| Output variance | Reduced from 0.0124 g^2 to 0.00071 g^2 |
| Accuracy | MAE of 0.035 g and RMSE of 0.048 g |
| Measurement range | 1 g to 10 kg |
| Temperature drift | Reduced from 0.42 g/deg C to below 0.03 g/deg C |
| Operating temperature range | 18-35 deg C |
| Power consumption | 28.4 mA active, 3.2 mA sleep |
| Estimated battery life | More than 36 hours under duty-cycled operation |

## Hardware Architecture

| Component | Specification / Role |
| --- | --- |
| Microcontroller | ATmega328P, 16 MHz |
| ADC module | HX711 24-bit ADC configured at 10 samples per second |
| Load cell | 10 kg single-point aluminum alloy strain-gauge load cell |
| Temperature sensor | DS18B20 1-Wire digital thermometer |
| Memory | AT24C256 I2C EEPROM for logging and parameter persistence |
| Power supply | 5 V AC adapter with MT3608 boost-converted Li-ion battery backup |

## Signal Processing

### Kalman Filtering

The firmware applies a scalar discrete-time Kalman filter to recursively estimate the true weight from noisy HX711 measurements. This provides adaptive smoothing while preserving useful response time for practical weighing.

### Multi-Point Calibration

Raw HX711 counts are converted to mass using ordinary least-squares regression from 10 reference calibration points. This improves linearity across the target measurement range.

### Temperature Compensation

Thermal drift is corrected using a first-order compensation model:

```text
W_compensated = W_raw - alpha * (T - T_ref)
```

where `alpha` is the measured temperature drift coefficient, `T` is the current temperature, and `T_ref` is the reference calibration temperature.

### Power Management

During inactivity, the ATmega328P transitions into Power-save mode and external modules are powered down where possible. This reduces current draw and extends field operation.

## Repository Contents

This checkout currently includes the main project report and supporting folder structure:

```text
.
|-- Final Year Project Placide, Laura, Rutanga 2026(1).pdf
|-- README.md
|-- docs/
|-- firmware/
|-- hardware/
`-- simulation/
```

The following locations are intended for implementation artifacts as the repository is completed:

| Path | Purpose |
| --- | --- |
| `firmware/main/main.ino` | Main Arduino measurement loop |
| `firmware/kalman/kalman_filter.h` | Scalar Kalman filter implementation |
| `firmware/calibration/calibration.h` | Calibration and EEPROM persistence logic |
| `firmware/temperature/temperature.h` | DS18B20 temperature measurement and drift correction |
| `firmware/power/power_mgmt.h` | Sleep mode and watchdog timer management |
| `firmware/display/display.h` | LCD and buzzer indicator routines |
| `simulation/kalman_matlab/kalman_sim.m` | MATLAB Kalman filter simulation |
| `simulation/proteus/` | Proteus circuit schematic files |
| `docs/` | Datasheets, circuit diagrams, calibration data, and report materials |

## Getting Started

1. Clone or download this repository.
2. Open the project report PDF for the full design, methodology, and results.
3. Review the hardware design before assembling the HX711, load cell, RC filter, power supply, and temperature sensor circuit.
4. Add or open the Arduino firmware under `firmware/main/main.ino` once available.
5. Compile and upload the sketch using Arduino IDE 2.x or a compatible AVR toolchain.
6. Perform multi-point calibration with known reference masses before using the scale for measurements.

## Authors

- DUSHIMERUGABA Placide (222002838)
- AGASARO Laura (222015111)
- RUTANGA Leonard (217256961)

## Acknowledgments

This project was conducted in partial fulfillment of a Bachelor of Science with Honors in Electronics and Telecommunication Engineering at the University of Rwanda, under the supervision of Assoc. Prof. Kizito NKURIKIYEYEZU.
