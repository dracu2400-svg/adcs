# STM32F303RCT6 Pin Mapping

## Overview
This document provides the complete pin assignment for the STM32F303RCT6 microcontroller used in the ADCS. The MCU is a 64-pin LQFP package.

**Note**: Actual pin assignments should be verified against the hardware schematic files located in `/hardware/sources/`. This document provides a recommended pin mapping based on the system requirements.

## MCU Package Information

- **Device**: STM32F303RCT6
- **Package**: LQFP64 (10mm × 10mm)
- **Core**: ARM Cortex-M4F @ 72 MHz
- **I/O Pins**: 51 available GPIO pins
- **ADC Channels**: 39 fast channels (5 MSPS)
- **Timers**: 10 timers (including advanced timers)
- **Communication**: 3× I2C, 3× SPI, 5× USART, 1× CAN

## Pin Assignment Table

### Power Pins

| Pin | Name | Function | Notes |
|-----|------|----------|-------|
| 1 | VBAT | Battery backup | Connect to VDD if no backup battery |
| 9 | VSSA | Analog ground | Connect to GND plane |
| 12 | VDD | Power supply | 3.3V, decoupling capacitor required |
| 13 | VSS | Ground | Connect to GND plane |
| 19 | VDDA | Analog power | 3.3V, separate filtering recommended |
| 24 | VSS | Ground | Connect to GND plane |
| 25 | VDD | Power supply | 3.3V, decoupling capacitor required |
| 36 | VSS | Ground | Connect to GND plane |
| 37 | VDD | Power supply | 3.3V, decoupling capacitor required |
| 48 | VSS | Ground | Connect to GND plane |
| 49 | VDD | Power supply | 3.3V, decoupling capacitor required |
| 60 | VSS | Ground | Connect to GND plane |
| 61 | VDD | Power supply | 3.3V, decoupling capacitor required |

### Clock Pins

| Pin | Name | Function | Configuration |
|-----|------|----------|---------------|
| 5 | PF0-OSC_IN | External crystal input | 8 MHz crystal |
| 6 | PF1-OSC_OUT | External crystal output | 8 MHz crystal |
| 3 | PC13 | RTC oscillator (optional) | 32.768 kHz crystal (optional) |

### Reset and Boot Pins

| Pin | Name | Function | Configuration |
|-----|------|----------|---------------|
| 7 | NRST | Reset input | Pull-up resistor, reset button, watchdog |
| 44 | BOOT0 | Boot mode selection | Pull-down resistor (boot from flash) |

### Debug Interface (SWD)

| Pin | Name | Function | Configuration |
|-----|------|----------|---------------|
| 46 | PA13 / SWDIO | Serial Wire Debug I/O | SWD programming/debug |
| 49 | PA14 / SWCLK | Serial Wire Clock | SWD programming/debug |
| 55 | PB3 / SWO | Serial Wire Output | Trace output (optional) |

## Peripheral Pin Assignments

### I2C1 - Housekeeping Sensors Bus

**Purpose**: Communication with INA226 (voltage/current sensors) and TMP100 (temperature sensors)

| Pin | Name | Function | Alt Function | Connected To |
|-----|------|----------|--------------|--------------|
| 58 | PB8 | I2C1_SCL | AF4 | All I2C sensors (SCL) |
| 59 | PB9 | I2C1_SDA | AF4 | All I2C sensors (SDA) |

**Configuration**:
- Speed: 400 kHz (Fast Mode I2C)
- Pull-up resistors: 4.7 kΩ external
- Devices: 4× INA226, 4× TMP100 (8 devices total)

### SPI1 - High-Speed Sensor Bus

**Purpose**: Communication with gyroscope and magnetometer

| Pin | Name | Function | Alt Function | Connected To |
|-----|------|----------|--------------|--------------|
| 31 | PA5 | SPI1_SCK | AF5 | Gyro, Magnetometer (SCK) |
| 32 | PA6 | SPI1_MISO | AF5 | Gyro, Magnetometer (MISO) |
| 33 | PA7 | SPI1_MOSI | AF5 | Gyro, Magnetometer (MOSI) |
| 29 | PA4 | SPI1_CS_GYRO | GPIO Output | Gyroscope chip select |
| 30 | PB0 | SPI1_CS_MAG | GPIO Output | Magnetometer chip select |

**Configuration**:
- Speed: Up to 18 MHz
- Mode: SPI Mode 0 or 3 (check sensor datasheets)
- Chip selects: Active low

### CAN Bus Interface

**Purpose**: Communication with OBC via PC-104 bus

| Pin | Name | Function | Alt Function | Connected To |
|-----|------|----------|--------------|--------------|
| 42 | PA11 | CAN_RX | AF9 | TCAN330GD CAN transceiver RX |
| 43 | PA12 | CAN_TX | AF9 | TCAN330GD CAN transceiver TX |

**Configuration**:
- Baud rate: 250 kbps or 500 kbps
- Transceiver: TCAN330GD
- Termination: 120Ω (optional, based on topology)

### Timer 1 - Magnetorquer PWM Generation

**Purpose**: Generate PWM signals for H-bridge drivers

| Pin | Name | Function | Alt Function | Connected To |
|-----|------|----------|--------------|--------------|
| 41 | PA8 | TIM1_CH1 | AF6 | H-Bridge X-axis PWM input |
| 42 | PA9 | TIM1_CH2 | AF6 | H-Bridge Y-axis PWM input |
| 43 | PA10 | TIM1_CH3 | AF6 | H-Bridge Z-axis PWM input |

**Configuration**:
- PWM Frequency: 20 kHz
- Resolution: 12-bit (3600 steps @ 72 MHz / 20 kHz)
- Duty cycle range: 0-100%

### GPIO - H-Bridge Control Signals

**Purpose**: Direction and enable control for magnetorquer H-bridges

| Pin | Name | Function | Connected To | Notes |
|-----|------|----------|--------------|-------|
| 16 | PC0 | GPIO Output | H-Bridge X IN1 | X-axis direction control |
| 17 | PC1 | GPIO Output | H-Bridge X IN2 | X-axis direction control |
| 18 | PC2 | GPIO Output | H-Bridge Y IN1 | Y-axis direction control |
| 19 | PC3 | GPIO Output | H-Bridge Y IN2 | Y-axis direction control |
| 34 | PB1 | GPIO Output | H-Bridge Z IN1 | Z-axis direction control |
| 35 | PB2 | GPIO Output | H-Bridge Z IN2 | Z-axis direction control |
| 56 | PB4 | GPIO Output | H-Bridge X EN | X-axis enable (optional) |
| 57 | PB5 | GPIO Output | H-Bridge Y EN | Y-axis enable (optional) |
| 58 | PB6 | GPIO Output | H-Bridge Z EN | Z-axis enable (optional) |

**Direction Control Logic**:
- IN1=1, IN2=0: Forward current
- IN1=0, IN2=1: Reverse current
- IN1=0, IN2=0: Brake
- IN1=1, IN2=1: Coast (high-Z)

### GPIO - H-Bridge Fault Detection

**Purpose**: Monitor H-bridge driver fault outputs

| Pin | Name | Function | Connected To | Notes |
|-----|------|----------|--------------|-------|
| 21 | PA0 | GPIO Input | H-Bridge X FAULT | Interrupt on fault |
| 22 | PA1 | GPIO Input | H-Bridge Y FAULT | Interrupt on fault |
| 23 | PA2 | GPIO Input | H-Bridge Z FAULT | Interrupt on fault |

**Configuration**:
- Input mode: Pull-up (fault is active low)
- Interrupt: Falling edge trigger
- Action: Disable PWM on fault

### Watchdog Interface

**Purpose**: External watchdog (TPS3823) communication

| Pin | Name | Function | Connected To | Notes |
|-----|------|----------|--------------|-------|
| 26 | PA3 | GPIO Output | Watchdog WDI | Watchdog kick signal |

**Configuration**:
- Output mode: Push-pull
- Initial state: Low
- Toggle rate: 10 Hz (every 100 ms)
- Timeout: 1600 ms

### USART1 - Debug Console (Optional)

**Purpose**: Debug logging and diagnostic interface

| Pin | Name | Function | Alt Function | Connected To |
|-----|------|----------|--------------|--------------|
| 46 | PA9 | USART1_TX | AF7 | Debug console TX |
| 47 | PA10 | USART1_RX | AF7 | Debug console RX (optional) |

**Configuration**:
- Baud rate: 115200 bps
- Data format: 8N1
- Flow control: None
- Connector: 2.54mm pin header

### ADC Inputs (Optional / Future Use)

**Purpose**: Analog measurements (e.g., sun sensors, voltage monitoring)

| Pin | Name | Function | ADC Channel | Purpose |
|-----|------|----------|-------------|---------|
| 10 | PC0 | ADC1_IN6 | ADC1 CH6 | Reserved / Sun sensor |
| 11 | PC1 | ADC1_IN7 | ADC1 CH7 | Reserved / Sun sensor |
| 14 | PC4 | ADC1_IN5 | ADC1 CH5 | Reserved |
| 15 | PC5 | ADC1_IN11 | ADC1 CH11 | Reserved |

**Configuration** (if used):
- Resolution: 12-bit
- Sampling rate: Up to 5 MSPS
- Reference: VDDA (3.3V)

### General Purpose I/O (Spare Pins)

| Pin | Name | Available | Notes |
|-----|------|-----------|-------|
| 38 | PB10 | Yes | Can be I2C2_SCL or GPIO |
| 39 | PB11 | Yes | Can be I2C2_SDA or GPIO |
| 51 | PB12 | Yes | Can be SPI2_CS or GPIO |
| 52 | PB13 | Yes | Can be SPI2_SCK or GPIO |
| 53 | PB14 | Yes | Can be SPI2_MISO or GPIO |
| 54 | PB15 | Yes | Can be SPI2_MOSI or GPIO |

## Pin Configuration Summary by Function

### Critical Real-Time Functions
- **SPI1 (Sensors)**: PA5, PA6, PA7, PA4, PB0 - High speed, low latency
- **TIM1 (PWM)**: PA8, PA9, PA10 - Precise timing for actuators
- **Watchdog**: PA3 - Must be serviced every 100 ms

### Communication Interfaces
- **I2C1**: PB8, PB9 - Housekeeping sensors
- **CAN**: PA11, PA12 - Primary command/telemetry link
- **USART1**: PA9, PA10 - Debug console (optional)

### Actuator Control
- **PWM Outputs**: PA8, PA9, PA10 (3 channels)
- **Direction Control**: PC0-PC3, PB1-PB2 (6 pins)
- **Enable Signals**: PB4-PB6 (3 pins, optional)
- **Fault Inputs**: PA0-PA2 (3 pins)

## Alternate Function Mapping

### STM32F303 Alternate Function Summary

| Peripheral | AF Number | Pins Used |
|------------|-----------|-----------|
| SPI1 | AF5 | PA5, PA6, PA7 |
| I2C1 | AF4 | PB8, PB9 |
| CAN | AF9 | PA11, PA12 |
| TIM1 | AF6 | PA8, PA9, PA10 |
| USART1 | AF7 | PA9, PA10 |

**Note**: PA9 and PA10 have dual assignments (USART1 and TIM1). If both are needed, use TIM1 on these pins and USART1 on alternate pins (e.g., USART2 on PA2/PA3).

## GPIO Configuration Code Example

```c
// Example configuration for STM32 HAL

// I2C1 - Sensor bus
GPIO_InitTypeDef GPIO_InitStruct = {0};
GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;  // PB8, PB9
GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

// SPI1 - High-speed sensors
GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;  // PA5-PA7
GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

// TIM1 - PWM outputs
GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;  // PA8-PA10
GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
GPIO_InitStruct.Alternate = GPIO_AF6_TIM1;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

// H-Bridge direction control
GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

// Watchdog kick
GPIO_InitStruct.Pin = GPIO_PIN_3;  // PA3
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
```

## Pin Usage Statistics

| Port | Pins Used | Pins Available | Utilization |
|------|-----------|----------------|-------------|
| PA | 16 | 16 | 100% |
| PB | 13 | 16 | 81% |
| PC | 6 | 8 | 75% |
| **Total** | **35** | **51** | **69%** |

**Margin**: 16 spare pins available for future expansion

## Design Recommendations

### PCB Layout Considerations

1. **High-Speed Signals** (SPI1):
   - Keep traces short (<50mm)
   - Match trace lengths for SCK, MOSI, MISO
   - Add series termination resistors (33Ω) on SCLK if needed
   - Ground plane underneath

2. **PWM Outputs** (TIM1):
   - Keep traces to H-bridges short
   - Add low-pass RC filters (100Ω + 100nF) near H-bridge inputs
   - Separate ground return paths for power and signal

3. **I2C Bus**:
   - External pull-up resistors: 4.7kΩ to 3.3V
   - Add series resistors (100Ω) for protection
   - Keep total bus capacitance <400pF

4. **Analog Pins** (ADC):
   - Separate VDDA filtering (LC filter)
   - Keep analog traces away from PWM and digital signals
   - Add input protection (Schottky diodes to GND and VDD)

5. **Power Decoupling**:
   - 100nF ceramic capacitor at each VDD pin
   - 10µF tantalum bulk capacitor near MCU
   - Separate VDDA filtering with ferrite bead

### ESD Protection
- Add TVS diodes on all external-facing signals (CAN, Debug UART)
- Add ESD diodes on sensor interfaces if connectorized

## Pin Conflicts and Resolutions

### Conflict 1: PA9 and PA10 (USART1 vs TIM1_CH2/CH3)
**Resolution**: Use TIM1 on PA9/PA10 (critical for PWM). Move USART1 to alternate pins if debug console is needed.

### Conflict 2: Multiple I2C devices on same bus
**Resolution**: Use unique I2C addresses for each device. All INA226 and TMP100 sensors have address pins for configuration.

## Verification Checklist

- [ ] All peripheral pins have correct alternate functions configured
- [ ] No pin conflicts between peripherals
- [ ] Adequate decoupling capacitors on all VDD pins
- [ ] Pull-up/pull-down resistors configured correctly
- [ ] PWM pins configured for correct timer channels
- [ ] I2C addresses are unique and correct
- [ ] SPI chip selects are independent GPIO pins
- [ ] Debug interface (SWD) is accessible
- [ ] Watchdog pin is correctly toggled
- [ ] Fault pins have interrupt capability

## Reference Documents

- STM32F303xC Datasheet (DS9118)
- STM32F303 Reference Manual (RM0316)
- Hardware Schematics: `/home/user/adcs/hardware/sources/3_mcu.SchDot`
- Pin Configuration Tool: STM32CubeMX
