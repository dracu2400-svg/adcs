# STM32L476RG Pin Mapping for ADCS

**Document Version:** 1.0
**Target MCU:** STM32L476RG (LQFP64 package)
**Last Updated:** 2025-11-18

## Important Notice

⚠️ **This document provides RECOMMENDED pin assignments based on STM32L476RG capabilities and ADCS peripheral requirements. These assignments MUST be verified against the actual hardware PCB design before use.**

The pin assignments in this document follow best practices for:
- Minimal pin conflicts
- Optimal peripheral placement
- Signal integrity considerations
- Hardware timer capabilities

**Always cross-reference with your actual hardware schematics before finalizing the firmware configuration.**

---

## Table of Contents

1. [Pin Assignment Summary](#pin-assignment-summary)
2. [Detailed Pin Descriptions](#detailed-pin-descriptions)
3. [Peripheral Configuration](#peripheral-configuration)
4. [Alternate Function Reference](#alternate-function-reference)
5. [Pin Configuration Guidelines](#pin-configuration-guidelines)
6. [Clock Tree Configuration](#clock-tree-configuration)

---

## Pin Assignment Summary

### Complete Pin Mapping Table

| Pin # | Port.Pin | Peripheral | Signal Name | AF | Direction | Description |
|-------|----------|------------|-------------|----|-----------|-----------|
| 1     | PC13     | GPIO       | -           | -  | -         | OSC32_IN (RTC crystal) |
| 2     | PC14     | GPIO       | -           | -  | -         | OSC32_OUT (RTC crystal) |
| 3     | PC15     | GPIO       | -           | -  | -         | Reserved |
| 4     | PH0      | RCC        | OSC_IN      | -  | Input     | External 8MHz crystal |
| 5     | PH1      | RCC        | OSC_OUT     | -  | Output    | External 8MHz crystal |
| 6     | NRST     | SYS        | RESET       | -  | Input     | System reset |
| 7     | PC0      | GPIO       | GPIO_0      | -  | I/O       | General purpose GPIO |
| 8     | PC1      | GPIO       | GPIO_1      | -  | I/O       | General purpose GPIO |
| 9     | PC2      | GPIO       | GPIO_2      | -  | I/O       | General purpose GPIO |
| 10    | PC3      | ADC        | SUN_SENS_0  | -  | Analog    | Sun sensor 0 input |
| 11    | VSSA     | PWR        | VSSA        | -  | Power     | Analog ground |
| 12    | VDDA     | PWR        | VDDA        | -  | Power     | Analog power (+3.3V) |
| 13    | PA0      | ADC        | SUN_SENS_1  | -  | Analog    | Sun sensor 1 input |
| 14    | PA1      | ADC        | SUN_SENS_2  | -  | Analog    | Sun sensor 2 input |
| 15    | PA2      | ADC        | SUN_SENS_3  | -  | Analog    | Sun sensor 3 input |
| 16    | PA3      | GPIO       | COIL_0_DIR  | -  | Output    | Coil 0 direction control |
| 17    | VSS      | PWR        | VSS         | -  | Power     | Digital ground |
| 18    | VDD      | PWR        | VDD         | -  | Power     | Digital power (+3.3V) |
| 19    | PA4      | GPIO       | COIL_1_DIR  | -  | Output    | Coil 1 direction control |
| 20    | PA5      | GPIO       | COIL_2_DIR  | -  | Output    | Coil 2 direction control |
| 21    | PA6      | GPIO       | COIL_0_EN   | -  | Output    | Coil 0 H-bridge enable |
| 22    | PA7      | GPIO       | COIL_1_EN   | -  | Output    | Coil 1 H-bridge enable |
| 23    | PC4      | GPIO       | COIL_2_EN   | -  | Output    | Coil 2 H-bridge enable |
| 24    | PC5      | GPIO       | WDI_EXT     | -  | Output    | External watchdog input |
| 25    | PB0      | TIM3_CH3   | COIL_0_PWM  | AF2| Output    | Coil 0 PWM output |
| 26    | PB1      | TIM3_CH4   | COIL_1_PWM  | AF2| Output    | Coil 1 PWM output |
| 27    | PB2      | GPIO       | -           | -  | -         | Reserved (BOOT1) |
| 28    | PB10     | TIM2_CH3   | COIL_2_PWM  | AF1| Output    | Coil 2 PWM output |
| 29    | PB11     | GPIO       | STATUS_LED  | -  | Output    | Status LED (optional) |
| 30    | VSS      | PWR        | VSS         | -  | Power     | Digital ground |
| 31    | VDD      | PWR        | VDD         | -  | Power     | Digital power (+3.3V) |
| 32    | PB12     | GPIO       | SPI1_CS_MAG | -  | Output    | Magnetometer CS (GPIO) |
| 33    | PB13     | SPI1       | SPI1_SCK    | AF5| Output    | SPI1 clock |
| 34    | PB14     | SPI1       | SPI1_MISO   | AF5| Input     | SPI1 MISO |
| 35    | PB15     | SPI1       | SPI1_MOSI   | AF5| Output    | SPI1 MOSI |
| 36    | PC6      | GPIO       | SPI1_CS_GYRO| -  | Output    | Gyroscope CS (GPIO) |
| 37    | PC7      | GPIO       | MAG_INT     | -  | Input     | Magnetometer interrupt |
| 38    | PC8      | GPIO       | GYRO_INT    | -  | Input     | Gyroscope interrupt |
| 39    | PC9      | GPIO       | -           | -  | -         | Reserved |
| 40    | PA8      | GPIO       | -           | -  | -         | Reserved (MCO) |
| 41    | PA9      | USART1     | USART1_TX   | AF7| Output    | Debug UART TX (optional) |
| 42    | PA10     | USART1     | USART1_RX   | AF7| Input     | Debug UART RX (optional) |
| 43    | PA11     | CAN1       | CAN1_RX     | AF9| Input     | CAN bus receive |
| 44    | PA12     | CAN1       | CAN1_TX     | AF9| Output    | CAN bus transmit |
| 45    | PA13     | SYS        | SWDIO       | AF0| I/O       | Debug interface (SWD) |
| 46    | VSS      | PWR        | VSS         | -  | Power     | Digital ground |
| 47    | VDD      | PWR        | VDD         | -  | Power     | Digital power (+3.3V) |
| 48    | PA14     | SYS        | SWCLK       | AF0| Input     | Debug interface (SWD) |
| 49    | PA15     | GPIO       | -           | -  | -         | Reserved (JTDI) |
| 50    | PC10     | GPIO       | -           | -  | -         | Reserved |
| 51    | PC11     | GPIO       | -           | -  | -         | Reserved |
| 52    | PC12     | GPIO       | -           | -  | -         | Reserved |
| 53    | PD2      | GPIO       | -           | -  | -         | Reserved |
| 54    | PB3      | SYS        | -           | -  | -         | Reserved (JTDO/TRACESWO) |
| 55    | PB4      | GPIO       | -           | -  | -         | Reserved (NJTRST) |
| 56    | PB5      | GPIO       | -           | -  | -         | Reserved |
| 57    | PB6      | I2C1       | I2C1_SCL    | AF4| Output    | I2C1 clock |
| 58    | PB7      | I2C1       | I2C1_SDA    | AF4| I/O       | I2C1 data |
| 59    | BOOT0    | SYS        | BOOT0       | -  | Input     | Boot mode select |
| 60    | PB8      | GPIO       | -           | -  | -         | Reserved |
| 61    | PB9      | GPIO       | -           | -  | -         | Reserved |
| 62    | VSS      | PWR        | VSS         | -  | Power     | Digital ground |
| 63    | VDD      | PWR        | VDD         | -  | Power     | Digital power (+3.3V) |
| 64    | -        | -          | -           | -  | -         | Not connected |

---

## Detailed Pin Descriptions

### Power Pins

| Pin | Name | Description |
|-----|------|-------------|
| 11  | VSSA | Analog ground - Connect to ground plane |
| 12  | VDDA | Analog supply (+3.3V) - Add 1µF + 10nF decoupling |
| 17, 30, 46, 62 | VSS | Digital ground - Connect to ground plane |
| 18, 31, 47, 63 | VDD | Digital supply (+3.3V) - Add 100nF decoupling on each |

**Decoupling Requirements:**
- Place 100nF ceramic capacitor close to each VDD pin
- Place 1µF + 10nF capacitors close to VDDA
- Use multiple vias to ground plane for all VSS connections

### Clock and Reset Pins

| Pin | Port.Pin | Signal | Description |
|-----|----------|--------|-------------|
| 1   | PC13 | OSC32_IN | 32.768 kHz crystal input (optional, for RTC) |
| 2   | PC14 | OSC32_OUT | 32.768 kHz crystal output (optional, for RTC) |
| 4   | PH0 | OSC_IN | 8 MHz main crystal input (optional, can use MSI) |
| 5   | PH1 | OSC_OUT | 8 MHz main crystal output |
| 6   | NRST | RESET | Active-low system reset (add 100nF + 10kΩ pullup) |

**Clock Source Options:**
1. **Internal MSI** (4 MHz default, recommended for low-power)
2. **External 8 MHz crystal** on PH0/PH1 (better accuracy)
3. **32.768 kHz RTC crystal** on PC13/PC14 (for RTC functionality)

### CAN Bus Interface (Primary Communication)

| Pin | Port.Pin | Signal | AF | Description |
|-----|----------|--------|----|-----------|
| 43  | PA11 | CAN1_RX | AF9 | CAN receive - Connect to CAN transceiver |
| 44  | PA12 | CAN1_TX | AF9 | CAN transmit - Connect to CAN transceiver |

**External Components Required:**
- CAN transceiver IC (e.g., TCAN330GD as per hardware design)
- 120Ω termination resistor (if board is at bus end)
- ESD protection on CANH/CANL lines

**Configuration:**
- Baud rate: 500 kbps (recommended for PC-104 bus)
- Bit timing: Configure for 80 MHz APB1 clock

### I2C Bus (Sensor Interface)

| Pin | Port.Pin | Signal | AF | Description |
|-----|----------|--------|----|-----------|
| 57  | PB6 | I2C1_SCL | AF4 | I2C clock - 100 kHz or 400 kHz |
| 58  | PB7 | I2C1_SDA | AF4 | I2C data - Open-drain with pullup |

**External Components Required:**
- 4.7kΩ pullup resistors on both SCL and SDA
- Optional: Series resistors (100Ω) for signal conditioning

**Connected Devices (I2C Addresses):**
- INA226 #0: 0x40 (Coil 0 current/voltage sensor)
- INA226 #1: 0x41 (Coil 1 current/voltage sensor)
- INA226 #2: 0x42 (Coil 2 current/voltage sensor)
- INA226 #3: 0x43 (Coil 3 current/voltage sensor)
- TMP100 #0: 0x48 (Coil 0 temperature sensor)
- TMP100 #1: 0x49 (Coil 1 temperature sensor)
- TMP100 #2: 0x4A (Coil 2 temperature sensor)
- TMP100 #3: 0x4B (Coil 3 temperature sensor)
- MMC5983MA: 0x30 (3-axis magnetometer, if using I2C mode)
- I3G4250D: 0x68 (3-axis gyroscope, if using I2C mode)

### SPI Bus (Alternative Sensor Interface)

| Pin | Port.Pin | Signal | AF | Description |
|-----|----------|--------|----|-----------|
| 33  | PB13 | SPI1_SCK | AF5 | SPI clock - Up to 40 MHz |
| 34  | PB14 | SPI1_MISO | AF5 | Master In Slave Out |
| 35  | PB15 | SPI1_MOSI | AF5 | Master Out Slave In |
| 32  | PB12 | CS_MAG | GPIO | Magnetometer chip select (active low) |
| 36  | PC6 | CS_GYRO | GPIO | Gyroscope chip select (active low) |

**Configuration:**
- Mode: SPI Master Mode 0 (CPOL=0, CPHA=0) or Mode 3 (check sensor datasheets)
- Clock: Up to 10 MHz for sensors (adjust based on sensor specifications)
- Data size: 8-bit
- NSS: Software mode (use GPIO for chip select)

**Note:** Choose either I2C or SPI for magnetometer and gyroscope based on your preference. SPI typically offers higher speed.

### PWM Outputs (Magnetorquer Control)

| Pin | Port.Pin | Timer | AF | Signal | Description |
|-----|----------|-------|----|-----------|-----------|
| 25  | PB0 | TIM3_CH3 | AF2 | COIL_0_PWM | X-axis coil PWM |
| 26  | PB1 | TIM3_CH4 | AF2 | COIL_1_PWM | Y-axis coil PWM |
| 28  | PB10 | TIM2_CH3 | AF1 | COIL_2_PWM | Z-axis coil PWM |

**PWM Configuration:**
- Frequency: ~20 kHz (above audible range, suitable for H-bridge)
- Resolution: 12-bit (4096 steps) at 20 kHz with 80 MHz clock
- Mode: PWM Mode 1 (output high when CNT < CCR)
- Polarity: Active high

**Timer Calculations:**
```
Timer Clock = 80 MHz (APB1 clock)
PWM Frequency = 20 kHz
Prescaler = 0 (PSC = 0)
Auto-Reload Register (ARR) = (80,000,000 / 20,000) - 1 = 3999
Resolution = 4000 steps (12-bit effective)
```

### GPIO Outputs (Magnetorquer Control)

| Pin | Port.Pin | Signal | Description |
|-----|----------|--------|-------------|
| 16  | PA3 | COIL_0_DIR | Coil 0 direction (H-bridge direction) |
| 19  | PA4 | COIL_1_DIR | Coil 1 direction (H-bridge direction) |
| 20  | PA5 | COIL_2_DIR | Coil 2 direction (H-bridge direction) |
| 21  | PA6 | COIL_0_EN | Coil 0 H-bridge enable (active high) |
| 22  | PA7 | COIL_1_EN | Coil 1 H-bridge enable (active high) |
| 23  | PC4 | COIL_2_EN | Coil 2 H-bridge enable (active high) |

**Configuration:**
- Mode: Push-pull output
- Speed: Low/Medium (2-10 MHz sufficient)
- Initial state: Low (disabled/forward direction)

**H-Bridge Control Logic:**
```
EN=0, DIR=X: H-bridge disabled (coil off)
EN=1, DIR=0: H-bridge enabled, forward current
EN=1, DIR=1: H-bridge enabled, reverse current
```

### ADC Inputs (Sun Sensors)

| Pin | Port.Pin | ADC Channel | Signal | Description |
|-----|----------|-------------|--------|-------------|
| 10  | PC3 | ADC1_IN4 | SUN_SENS_0 | Sun sensor photodiode 0 |
| 13  | PA0 | ADC1_IN5 | SUN_SENS_1 | Sun sensor photodiode 1 |
| 14  | PA1 | ADC1_IN6 | SUN_SENS_2 | Sun sensor photodiode 2 |
| 15  | PA2 | ADC1_IN7 | SUN_SENS_3 | Sun sensor photodiode 3 |

**ADC Configuration:**
- Resolution: 12-bit (0-4095)
- Reference: VDDA (3.3V)
- Sampling time: 47.5 cycles (adjust based on source impedance)
- Mode: Single conversion or scan mode
- Optional: Use DMA for automatic data transfer

**External Circuit:**
- Photodiode with transimpedance amplifier
- Low-pass filter to reduce noise
- Voltage scaling to 0-3.3V range

### Watchdog Output

| Pin | Port.Pin | Signal | Description |
|-----|----------|--------|-------------|
| 24  | PC5 | WDI_EXT | External watchdog input (toggle periodically) |

**Configuration:**
- Mode: Push-pull output
- Speed: Low
- Function: Toggle this pin periodically to prevent watchdog reset
- Typical frequency: 1-10 Hz square wave

**External Watchdog IC:**
- Connected IC: TPS3823 (as per hardware design)
- Timing: Check TPS3823 datasheet for required toggle timing

### Debug Interface (SWD)

| Pin | Port.Pin | Signal | Description |
|-----|----------|--------|-------------|
| 45  | PA13 | SWDIO | Serial Wire Debug Data I/O |
| 48  | PA14 | SWCLK | Serial Wire Debug Clock |
| 6   | NRST | RESET | System reset (used during debug) |

**Configuration:**
- Always enabled by default (no AF configuration needed)
- Connect to ST-Link or J-Link debugger
- Do NOT use PA13/PA14 for other functions unless absolutely necessary

### UART (Optional Debug Console)

| Pin | Port.Pin | Signal | AF | Description |
|-----|----------|--------|----|-----------|
| 41  | PA9 | USART1_TX | AF7 | Debug console transmit |
| 42  | PA10 | USART1_RX | AF7 | Debug console receive |

**Configuration:**
- Baud rate: 115200 bps (common debug rate)
- Data: 8 bits, No parity, 1 stop bit (8N1)
- Flow control: None
- Optional: Can be disabled to save power

### General Purpose GPIOs

| Pin | Port.Pin | Signal | Description |
|-----|----------|--------|-------------|
| 7   | PC0 | GPIO_0 | General purpose I/O |
| 8   | PC1 | GPIO_1 | General purpose I/O |
| 9   | PC2 | GPIO_2 | General purpose I/O |
| 29  | PB11 | STATUS_LED | Optional status LED output |

**Suggested Uses:**
- GPIO_0/1/2: Future expansion, test points
- STATUS_LED: Heartbeat or status indication

### Sensor Interrupt Inputs

| Pin | Port.Pin | Signal | Description |
|-----|----------|--------|-------------|
| 37  | PC7 | MAG_INT | Magnetometer data ready interrupt (optional) |
| 38  | PC8 | GYRO_INT | Gyroscope data ready interrupt (optional) |

**Configuration:**
- Mode: Input with pullup/pulldown as needed
- EXTI: Configure for rising/falling edge interrupt
- Use: Trigger sensor data read when new data available

---

## Peripheral Configuration

### Summary of Peripherals Used

| Peripheral | Pins Used | Purpose |
|------------|-----------|---------|
| CAN1 | PA11, PA12 | Primary communication bus |
| I2C1 | PB6, PB7 | Sensor communication (current, temp, IMU) |
| SPI1 | PB13, PB14, PB15, PB12, PC6 | Alternative IMU communication |
| TIM2 | PB10 | PWM for magnetorquer coil 2 |
| TIM3 | PB0, PB1 | PWM for magnetorquer coils 0 and 1 |
| ADC1 | PC3, PA0, PA1, PA2 | Sun sensor analog inputs |
| USART1 | PA9, PA10 | Debug console (optional) |
| SWD | PA13, PA14 | Debug interface |
| GPIO | Various | Control signals, status, expansion |

---

## Alternate Function Reference

### Quick Reference for Alternate Functions

| Peripheral | Pin | AF Number | Notes |
|------------|-----|-----------|-------|
| CAN1_RX | PA11 | AF9 | |
| CAN1_TX | PA12 | AF9 | |
| I2C1_SCL | PB6 | AF4 | Open-drain, needs pullup |
| I2C1_SDA | PB7 | AF4 | Open-drain, needs pullup |
| SPI1_SCK | PB13 | AF5 | |
| SPI1_MISO | PB14 | AF5 | |
| SPI1_MOSI | PB15 | AF5 | |
| TIM2_CH3 | PB10 | AF1 | PWM output |
| TIM3_CH3 | PB0 | AF2 | PWM output |
| TIM3_CH4 | PB1 | AF2 | PWM output |
| USART1_TX | PA9 | AF7 | Optional debug |
| USART1_RX | PA10 | AF7 | Optional debug |

### Configuring Alternate Functions

To configure a pin for alternate function in STM32 HAL:

```c
GPIO_InitTypeDef GPIO_InitStruct = {0};

// Example: Configure PA11 for CAN1_RX
GPIO_InitStruct.Pin = GPIO_PIN_11;
GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;        // Alternate function push-pull
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;     // AF9 for CAN1
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
```

---

## Pin Configuration Guidelines

### GPIO Output Configuration

**For digital outputs (enable, direction, LEDs):**
```c
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;    // Push-pull
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;   // Low speed sufficient for control signals
```

### GPIO Input Configuration

**For digital inputs (interrupts, buttons):**
```c
GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
GPIO_InitStruct.Pull = GPIO_PULLUP;            // Or PULLDOWN/NOPULL as needed
```

**For interrupt inputs:**
```c
GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;    // Or FALLING, or RISING_FALLING
GPIO_InitStruct.Pull = GPIO_PULLUP;
HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
// Then configure and enable EXTI interrupt
```

### Analog Input Configuration

**For ADC inputs:**
```c
GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
GPIO_InitStruct.Pull = GPIO_NOPULL;            // No pull resistors for analog
```

### I2C Pin Configuration

**For I2C (open-drain):**
```c
GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;        // Alternate function open-drain
GPIO_InitStruct.Pull = GPIO_PULLUP;            // Internal pullup (external also needed)
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
```

---

## Clock Tree Configuration

### Recommended Clock Configuration for ADCS

**System Clock:** 80 MHz (Maximum for STM32L476RG)

```
Clock Source: MSI (4 MHz) → PLL
PLL Configuration:
  - PLLM = 1 (divider)
  - PLLN = 40 (multiplier)
  - PLLR = 2 (divider for system clock)
  - Result: 4 MHz * 40 / 2 = 80 MHz

AHB Clock (HCLK): 80 MHz (no division)
APB1 Clock (PCLK1): 80 MHz (no division) - Used for CAN, I2C, TIM2-7
APB2 Clock (PCLK2): 80 MHz (no division) - Used for SPI1, USART1, TIM1
```

**Peripheral Clock Enables:**
```c
__HAL_RCC_GPIOA_CLK_ENABLE();
__HAL_RCC_GPIOB_CLK_ENABLE();
__HAL_RCC_GPIOC_CLK_ENABLE();
__HAL_RCC_CAN1_CLK_ENABLE();
__HAL_RCC_I2C1_CLK_ENABLE();
__HAL_RCC_SPI1_CLK_ENABLE();
__HAL_RCC_TIM2_CLK_ENABLE();
__HAL_RCC_TIM3_CLK_ENABLE();
__HAL_RCC_ADC_CLK_ENABLE();
// Optional:
__HAL_RCC_USART1_CLK_ENABLE();
```

---

## PCB Layout Recommendations

### Critical Routing Guidelines

1. **CAN Bus:**
   - Route CAN_RX and CAN_TX as differential pair to transceiver
   - Keep traces short and equal length
   - Place 120Ω termination close to transceiver

2. **I2C Bus:**
   - Keep SCL and SDA traces short (<30 cm for 400 kHz)
   - Route together, avoid crossing other high-speed signals
   - Place pullup resistors close to MCU

3. **SPI Bus:**
   - Route as controlled impedance if using high speeds (>10 MHz)
   - Keep CS signals short and route separately
   - Star topology for CS pins if multiple slaves

4. **PWM Outputs:**
   - Add series resistors (10-100Ω) close to MCU for EMI reduction
   - Route away from sensitive analog signals
   - Consider RC snubbers on H-bridge inputs

5. **ADC Inputs:**
   - Route analog traces away from digital signals
   - Use ground guard traces if possible
   - Place filter capacitors close to MCU pins
   - Star ground connection to VSSA

6. **Power:**
   - Place decoupling caps as close as possible to VDD pins
   - Use multiple vias for VDD/VSS connections
   - Separate analog and digital ground planes if possible, join at one point

### ESD Protection

Add ESD protection on:
- CAN_RX, CAN_TX
- External connector pins
- Any pins connected to external sensors

---

## Verification Checklist

Before finalizing firmware configuration:

- [ ] Verify all pin assignments match actual PCB layout
- [ ] Check that no pins have conflicting assignments
- [ ] Confirm all peripheral alternate functions are correct
- [ ] Verify external pullup/pulldown resistors match GPIO configuration
- [ ] Check that ADC inputs have appropriate filtering
- [ ] Confirm PWM timer channels match H-bridge inputs
- [ ] Verify CAN transceiver connections
- [ ] Check I2C pullup resistor values (typically 4.7kΩ)
- [ ] Confirm debug interface (SWD) is accessible
- [ ] Verify power pin decoupling capacitors are placed
- [ ] Check clock source configuration (crystal values)
- [ ] Verify boot mode pin (BOOT0) configuration

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-11-18 | ADCS Team | Initial pin mapping document |

---

## References

1. [STM32L476RG Datasheet](https://www.st.com/resource/en/datasheet/stm32l476rg.pdf) - Table 16: Alternate function mapping
2. [STM32L476RG Reference Manual RM0351](https://www.st.com/resource/en/reference_manual/rm0351-stm32l47xxx-stm32l48xxx-stm32l49xxx-and-stm32l4axxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
3. [AN4899: STM32 GPIO configuration for hardware settings and low-power consumption](https://www.st.com/resource/en/application_note/dm00315319-stm32-gpio-configuration-for-hardware-settings-and-lowpower-consumption-stmicroelectronics.pdf)
4. ADCS Hardware Schematics (hardware/sources/)

---

**Document maintained by:** ADCS Firmware Team
**For updates or corrections, please update this file and increment the version number.**
