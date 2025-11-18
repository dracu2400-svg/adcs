# Interfaces Documentation

## Overview
This document describes all electrical and communication interfaces for the ADCS system, including internal sensor/actuator interfaces and external satellite bus interfaces.

## External Interfaces

### 1. PC-104 Mechanical and Electrical Interface

#### 1.1 Mechanical Interface
- **Form Factor**: PC-104 (ISA-style connector)
- **Board Dimensions**: 90 mm × 90 mm
- **Mounting Holes**: 4 corners, M3 standoffs
- **Stack Height**: TBD (compliant with 3U CubeSat)
- **Connector**: Standard PC-104 connector (stackthrough)

#### 1.2 Power Interface (PC-104 Bus)
- **Input Voltage**: +5V, +12V (from satellite power bus)
- **Power Pins**: Defined by PC-104 standard
- **Expected Current Draw**:
  - +5V: ~200 mA (MCU, sensors)
  - +12V: ~500 mA peak (magnetorquer actuation)
  - Idle: <50 mA total
- **Protection**:
  - Reverse polarity protection
  - Over-voltage protection
  - Inrush current limiting

#### 1.3 Signal Interface (PC-104 Bus)
While the PC-104 connector provides multiple signal lines, the ADCS primarily uses:
- **CAN Bus Signals**: CAN_H, CAN_L, GND
- **Reset Signal**: System reset from OBC
- **Power Good**: Status to indicate ADCS ready

### 2. CAN Bus Interface

#### 2.1 Physical Layer
- **Transceiver IC**: TCAN330GD (Texas Instruments)
- **Topology**: Differential pair (CAN_H, CAN_L)
- **Termination**: 120Ω termination resistor (optional, configurable)
- **Cable**: Twisted pair
- **Connector**: Via PC-104 bus

#### 2.2 Electrical Specifications
- **Logic Levels**:
  - Recessive: 0V differential
  - Dominant: 2V typical differential
- **Common Mode Voltage**: 2.5V nominal
- **Bit Rate**: 250 kbps or 500 kbps (configurable)
- **Max Cable Length**: ~40m @ 250 kbps

#### 2.3 Protocol Layer
- **Standard**: CAN 2.0B (Extended Frame)
- **Frame Type**: Data frames, Remote frames
- **Identifier**: 29-bit extended ID
- **DLC**: 0-8 bytes data length
- **Error Detection**: CRC-15, ACK, bit stuffing

#### 2.4 Message Definitions

##### Command Messages (OBC → ADCS)
| ID (Hex) | Name | DLC | Description |
|----------|------|-----|-------------|
| 0x100 | MODE_CHANGE | 1 | Set operating mode (0=Idle, 1=Detumble, 2=Point) |
| 0x101 | SET_TARGET | 8 | Set target attitude (quaternion, partial) |
| 0x102 | REQUEST_TLM | 1 | Request specific telemetry packet |
| 0x103 | CALIBRATE | 2 | Trigger sensor calibration (sensor ID + command) |
| 0x104 | CONFIG_UPDATE | 8 | Update configuration parameter |
| 0x105 | RESET | 0 | Software reset command |

##### Telemetry Messages (ADCS → OBC)
| ID (Hex) | Name | DLC | Rate | Description |
|----------|------|-----|------|-------------|
| 0x200 | HEARTBEAT | 1 | 1 Hz | Status byte (mode, health flags) |
| 0x201 | ATTITUDE | 8 | 1 Hz | Attitude quaternion (2 bytes × 4) |
| 0x202 | ANGULAR_RATES | 6 | 1 Hz | Gyroscope data (3 axes × 2 bytes) |
| 0x203 | MAG_FIELD | 6 | 1 Hz | Magnetometer data (3 axes × 2 bytes) |
| 0x204 | HOUSEKEEPING_1 | 8 | 1 Hz | Voltages and currents |
| 0x205 | HOUSEKEEPING_2 | 8 | 1 Hz | Temperatures and power |
| 0x206 | CONTROL_OUTPUT | 6 | 0.1 Hz | Magnetorquer commands (3 axes × 2 bytes) |
| 0x2FF | ERROR_LOG | 8 | Event | Error code and details |

#### 2.5 Data Packing Format
- **Integers**: Little-endian byte order
- **Fixed-Point**: Q15 format for normalized values (e.g., quaternions)
- **Floating-Point**: Not used (due to bandwidth constraints)
- **Scaling Examples**:
  - Angular rate: ±2000 °/s → 16-bit signed (0.061 °/s resolution)
  - Magnetic field: ±8 Gauss → 16-bit signed (0.244 mG resolution)
  - Quaternion: ±1 → 16-bit signed Q15 (0.00003 resolution)

## Internal Sensor Interfaces

### 3. I2C Bus Interface

#### 3.1 Bus Configuration
- **Master**: STM32F303 MCU
- **Speed**: 400 kHz (Fast Mode I2C)
- **Voltage Level**: 3.3V
- **Pull-up Resistors**: 4.7 kΩ to 3.3V
- **SCL Pin**: MCU GPIO (TBD)
- **SDA Pin**: MCU GPIO (TBD)

#### 3.2 Connected Devices

##### Voltage Sensors (4x INA226)
- **Device**: Texas Instruments INA226
- **I2C Addresses**:
  - INA226_1 (Coil X): 0x40 (1000000b)
  - INA226_2 (Coil Y): 0x41 (1000001b)
  - INA226_3 (Coil Z): 0x44 (1000100b)
  - INA226_4 (Bus): 0x45 (1000101b)
- **Registers**:
  - 0x00: Configuration
  - 0x01: Shunt Voltage
  - 0x02: Bus Voltage
  - 0x04: Current
  - 0x05: Power
  - 0x06: Alert/Mask Enable
- **Measurement Range**:
  - Bus Voltage: 0-36V (81.92 mV LSB)
  - Shunt Voltage: ±81.92 mV (2.5 µV LSB)
  - Configurable shunt resistor (e.g., 0.1Ω)

##### Current Sensors (4x INA226)
- Same as voltage sensors above (combined voltage/current monitor)

##### Temperature Sensors (4x TMP100)
- **Device**: Texas Instruments TMP100
- **I2C Addresses**:
  - TMP100_1 (MCU): 0x48 (1001000b)
  - TMP100_2 (Coil X): 0x49 (1001001b)
  - TMP100_3 (Coil Y): 0x4A (1001010b)
  - TMP100_4 (Coil Z): 0x4B (1001011b)
- **Registers**:
  - 0x00: Temperature (12-bit)
  - 0x01: Configuration
  - 0x02: T_LOW threshold
  - 0x03: T_HIGH threshold
- **Resolution**: 12-bit (0.0625°C per LSB)
- **Range**: -55°C to +125°C

##### Magnetometer (MMC5983MA) - Optional I2C
- **Device**: MEMSIC MMC5983MA
- **I2C Address**: 0x30 (default)
- **Registers**: See SPI interface section (similar register map)
- **Alternative**: Can use SPI instead

##### Gyroscope (I3G4250DTR) - Optional I2C
- **Device**: STMicroelectronics I3G4250DTR
- **I2C Address**: 0x68 or 0x69 (configurable by SA0 pin)
- **Registers**: See SPI interface section
- **Alternative**: Can use SPI instead (recommended for speed)

#### 3.3 I2C Transaction Examples

##### Read Temperature from TMP100
```
START → ADDR(0x48) + W → ACK → REG(0x00) → ACK →
START → ADDR(0x48) + R → ACK → DATA_HIGH → ACK → DATA_LOW → NACK → STOP
```

##### Read Voltage from INA226
```
START → ADDR(0x40) + W → ACK → REG(0x02) → ACK →
START → ADDR(0x40) + R → ACK → DATA_HIGH → ACK → DATA_LOW → NACK → STOP
```

### 4. SPI Bus Interface

#### 4.1 Bus Configuration
- **Master**: STM32F303 MCU
- **Speed**: Up to 18 MHz (72 MHz / 4)
- **Mode**: SPI Mode 3 (CPOL=1, CPHA=1) - check sensor datasheet
- **Voltage Level**: 3.3V
- **SCLK Pin**: MCU GPIO (TBD)
- **MOSI Pin**: MCU GPIO (TBD)
- **MISO Pin**: MCU GPIO (TBD)
- **CS Pins**: Separate GPIO per device

#### 4.2 Connected Devices

##### Gyroscope (I3G4250DTR)
- **Device**: STMicroelectronics I3G4250DTR (3-axis gyroscope)
- **CS Pin**: MCU GPIO (TBD)
- **SPI Mode**: Mode 0 or Mode 3 (CPOL=0/1, CPHA=0/1)
- **Max Speed**: 10 MHz
- **Data Order**: MSB first
- **Key Registers**:
  - 0x0F: WHO_AM_I (0xD3)
  - 0x20: CTRL_REG1 (power, ODR, enable)
  - 0x23: CTRL_REG4 (full scale, endian)
  - 0x28-0x2D: OUT_X_L/H, OUT_Y_L/H, OUT_Z_L/H (angular rates)
- **Full Scale Range**: ±245/500/2000 dps (configurable)
- **Output Data Rate**: Up to 800 Hz

##### Magnetometer (MMC5983MA)
- **Device**: MEMSIC MMC5983MA (3-axis magnetometer)
- **CS Pin**: MCU GPIO (TBD)
- **SPI Mode**: Mode 0 (CPOL=0, CPHA=0)
- **Max Speed**: 10 MHz
- **Data Order**: MSB first
- **Key Registers**:
  - 0x00-0x05: Xout, Yout, Zout (2 bytes each, 18-bit data)
  - 0x08: Status
  - 0x09: Internal Control 0
  - 0x0A: Internal Control 1
  - 0x0B: Internal Control 2
  - 0x2F: Product ID
- **Resolution**: 18-bit
- **Range**: ±8 Gauss
- **Noise**: 0.4 mG RMS @ 100 Hz

#### 4.3 SPI Transaction Examples

##### Read Gyroscope (I3G4250DTR)
```
CS Low → TX: 0xE8 (Read | Auto-increment | 0x28) →
RX: Dummy → TX: 0x00 → RX: OUT_X_L →
TX: 0x00 → RX: OUT_X_H → ... (6 bytes total) → CS High
```

##### Read Magnetometer (MMC5983MA)
```
CS Low → TX: 0x00 (Read X_out[17:10]) → RX: Dummy →
TX: 0x00 → RX: X_MSB → ... (6 bytes total) → CS High
```

### 5. Sun Sensor Interface (TBD)
- **Interface Type**: TBD (likely analog or digital)
- **Connector**: TBD
- **Signal Type**: TBD
- **Voltage Level**: TBD
- **Protocol**: TBD

## Internal Actuator Interfaces

### 6. Magnetorquer H-Bridge Interface

#### 6.1 H-Bridge Driver (DRV8834PWP)
- **Driver IC**: Texas Instruments DRV8834PWP (or equivalent)
- **Configuration**: 3 independent H-bridges (one per axis)
- **Control Method**: PWM with direction control

#### 6.2 Control Signals (Per Axis)

##### X-Axis Magnetorquer
- **PWM Signal**: Timer PWM output (e.g., TIM1_CH1)
  - Frequency: 20 kHz (ultrasonic, avoid audible noise)
  - Duty Cycle: 0-100%
  - Voltage: 3.3V logic
- **Direction Pin 1 (IN1)**: MCU GPIO (TBD)
- **Direction Pin 2 (IN2)**: MCU GPIO (TBD)
  - IN1=1, IN2=0: Forward current
  - IN1=0, IN2=1: Reverse current
  - IN1=0, IN2=0: Brake (short both sides)
  - IN1=1, IN2=1: Coast (high-Z)
- **Enable Pin (EN)**: MCU GPIO or tied high
- **Fault Pin**: MCU GPIO input (active low on fault)
- **Current Sense**: Via INA226 on I2C bus

##### Y-Axis Magnetorquer
- Similar to X-axis, different timer channel (e.g., TIM1_CH2)

##### Z-Axis Magnetorquer
- Similar to X-axis, different timer channel (e.g., TIM1_CH3)

#### 6.3 Current Feedback Loop
- **Sensing**: INA226 measures current through each magnetorquer
- **Update Rate**: 100 Hz
- **Control**: PI controller adjusts PWM duty cycle to match setpoint
- **Range**: 0 to ~1A per coil (TBD based on coil design)

#### 6.4 Protection Features
- **Over-Current**: Shutdown via software (INA226 monitoring)
- **Over-Temperature**: Shutdown via software (TMP100 monitoring)
- **Fault Pin**: Hardware fault detection from DRV8834
- **Thermal Shutdown**: Built into DRV8834 IC

### 7. Watchdog Interface

#### 7.1 External Watchdog (TPS3823)
- **IC**: Texas Instruments TPS3823
- **Purpose**: Monitor MCU health, reset on failure

#### 7.2 Signals
- **WDI (Watchdog Input)**: MCU GPIO output
  - Function: Watchdog kick signal
  - Timing: Must toggle within 1600 ms
  - MCU Pin: TBD
- **RESET (Reset Output)**: Connected to MCU NRST pin
  - Function: Assert MCU reset on watchdog timeout
  - Active Low
- **VDD Monitor**: Monitors MCU power supply
  - Threshold: ~2.85V typical
  - Action: Assert reset if VDD drops below threshold

#### 7.3 Timing Requirements
- **Minimum Kick Interval**: >1 ms
- **Maximum Kick Interval**: <1600 ms (typ)
- **Reset Pulse Width**: ~400 ms
- **Typical Kick Rate**: 10 Hz (100 ms period)

## Debugging and Programming Interfaces

### 8. SWD (Serial Wire Debug) Interface

#### 8.1 Connector
- **Type**: 2.54mm pin header (4-pin minimum)
- **Pinout**:
  1. VCC (3.3V)
  2. SWDIO (Serial Wire Data I/O)
  3. SWCLK (Serial Wire Clock)
  4. GND

#### 8.2 Optional Pins
- **NRST**: Reset pin (for hardware reset during debug)
- **SWO**: Serial Wire Output (for printf debugging)

#### 8.3 Debugger
- **Supported**: ST-Link V2/V3, J-Link, CMSIS-DAP
- **Protocol**: SWD (Serial Wire Debug, ARM standard)

### 9. UART Debug Interface (Optional)

#### 9.1 Configuration
- **UART**: USART1 or USART2 (TBD)
- **Baud Rate**: 115200 bps
- **Data Format**: 8N1 (8 data bits, no parity, 1 stop bit)
- **Flow Control**: None

#### 9.2 Signals
- **TX**: MCU UART TX pin
- **RX**: MCU UART RX pin (optional, for command input)
- **GND**: Common ground

#### 9.3 Purpose
- Debug logging (printf statements)
- Interactive command line (optional)
- Diagnostics and testing

## Interface Summary Table

| Interface | Type | Devices | Speed | Purpose |
|-----------|------|---------|-------|---------|
| PC-104 | Parallel Bus | OBC | - | Power, CAN signals |
| CAN | Differential Serial | OBC | 250-500 kbps | Commands, telemetry |
| I2C | Serial Bus | INA226 (8×), TMP100 (4×) | 400 kHz | Housekeeping sensors |
| SPI | Serial Bus | Gyro, Mag | 10 MHz | High-speed sensors |
| PWM | Timer Output | H-Bridge (3×) | 20 kHz | Magnetorquer control |
| GPIO | Digital I/O | H-Bridge control | - | Direction, enable, fault |
| Watchdog | Digital I/O | TPS3823 | - | System health monitoring |
| SWD | Debug | Debugger | - | Programming, debugging |
| UART | Serial | Debug Console | 115200 bps | Logging, diagnostics |

## Connector and Cable Specifications

### External Connectors
- **PC-104**: Standard PC-104 stackthrough connector
- **Magnetorquers**: TBD (wire-to-board or screw terminal)
- **Debug Header**: 2.54mm pin header (4-10 pins)

### Internal Connectors
- **Sensors**: Surface-mount (on-board) - no connectors
- **H-Bridges**: Surface-mount driver ICs

### Wiring
- **Magnetorquer Coils**:
  - Wire Gauge: TBD (based on current rating ~1A)
  - Length: TBD (depends on CubeSat layout)
  - Shielding: Not required (low frequency)
- **CAN Bus**:
  - Twisted pair (via PC-104 bus)
  - Termination: 120Ω (if end node)
