# Data Path Architecture

## Overview
This document describes the data flow paths within the ADCS system, from sensor acquisition through processing to actuator commands and telemetry output.

## Data Flow Diagram

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│  Gyroscope   │────▶│              │     │              │
│  I3G4250DTR  │     │              │     │   Attitude   │
└──────────────┘     │              │────▶│  Estimation  │
                     │    Sensor    │     │    Module    │
┌──────────────┐     │   Reading    │     │              │
│ Magnetometer │────▶│     Task     │     │  (Fusion)    │
│  MMC5983MA   │     │              │     └──────┬───────┘
└──────────────┘     │   (100 Hz)   │            │
                     │              │            │
┌──────────────┐     │              │            │
│  Sun Sensor  │────▶│              │            │
│    (TBD)     │     └──────────────┘            │
└──────────────┘                                 │
                                                 ▼
┌──────────────┐                     ┌──────────────────┐
│   Voltage    │                     │    Control       │
│   Sensors    │                     │   Algorithm      │
│  (4x INA226) │────┐                │                  │
└──────────────┘    │                │  - Detumbling    │
                    │   ┌────────┐   │  - Pointing      │
┌──────────────┐    ├──▶│ House- │   │  - PID/B-dot     │
│   Current    │    │   │keeping │   │                  │
│   Sensors    │────┤   │  Task  │   │   (10 Hz)        │
│  (4x INA226) │    │   │        │   └────────┬─────────┘
└──────────────┘    │   └────────┘            │
                    │                         │
┌──────────────┐    │                         │
│ Temperature  │    │                         ▼
│   Sensors    │────┘             ┌─────────────────────┐
│  (4x TMP100) │                  │   Magnetorquer      │
└──────────────┘                  │   Driver Module     │
                                  │                     │
                                  │  - PWM Generation   │
        ┌─────────────────────────┤  - H-Bridge Control │
        │                         │  - Current Feedback │
        │                         └─────────────────────┘
        │                                     │
        │                                     ▼
        │                         ┌─────────────────────┐
        │                         │  Magnetorquers      │
        │                         │  (X, Y, Z axes)     │
        │                         └─────────────────────┘
        │
        │  ┌──────────────┐       ┌─────────────────────┐
        └─▶│  Telemetry   │──────▶│   CAN Bus TX        │
           │   Module     │       │   (TCAN330GD)       │
           │   (1 Hz)     │       └─────────┬───────────┘
           └──────────────┘                 │
                ▲                           │
                │                           ▼
                │                 ┌─────────────────────┐
           ┌────┴────────┐        │   OBC / Ground      │
           │   Command   │◀───────│   Commands (CAN RX) │
           │   Handler   │        └─────────────────────┘
           └─────────────┘
```

## Detailed Data Paths

### 1. Sensor Data Path

#### 1.1 Gyroscope Data Path (I3G4250DTR)
**Flow**: Gyroscope → SPI/I2C → MCU → Processing → Control

1. **Acquisition**:
   - Rate: 100 Hz (10 ms sampling)
   - Interface: SPI or I2C
   - Data: 3-axis angular rates (ωx, ωy, ωz)
   - Resolution: 16-bit per axis
   - Units: °/s or rad/s

2. **Processing**:
   - Byte order conversion (endianness)
   - Scaling to physical units
   - Bias subtraction (calibration)
   - Low-pass filtering (optional)
   - Temperature compensation

3. **Storage**:
   - Latest reading: Global variable
   - History buffer: Circular buffer (for filtering)
   - Size: ~10 samples (100 bytes)

4. **Consumers**:
   - Attitude estimation module
   - Detumbling controller
   - Telemetry module

#### 1.2 Magnetometer Data Path (MMC5983MA)
**Flow**: Magnetometer → SPI/I2C → MCU → Processing → Control

1. **Acquisition**:
   - Rate: 100 Hz (10 ms sampling)
   - Interface: SPI or I2C
   - Data: 3-axis magnetic field (Bx, By, Bz)
   - Resolution: 18-bit per axis
   - Units: Gauss or Tesla

2. **Processing**:
   - Byte order conversion
   - Scaling to physical units
   - Hard iron compensation (offset removal)
   - Soft iron compensation (ellipsoid correction)
   - Noise filtering

3. **Storage**:
   - Latest reading: Global variable
   - Calibration parameters: Non-volatile memory
   - Size: ~12 bytes current + 36 bytes calibration

4. **Consumers**:
   - Attitude estimation module
   - Pointing controller
   - B-dot algorithm (derivative)
   - Telemetry module

#### 1.3 Sun Sensor Data Path (TBD)
**Flow**: Sun Sensor → Interface → MCU → Processing → Attitude Estimation

1. **Acquisition**: TBD
2. **Processing**: TBD
3. **Storage**: TBD
4. **Consumers**: Attitude estimation, telemetry

#### 1.4 Housekeeping Sensor Data Path

##### Voltage Sensors (4x INA226)
**Flow**: INA226 → I2C → MCU → Monitoring → Telemetry

1. **Acquisition**:
   - Rate: 1 Hz (slow monitoring)
   - Interface: I2C (shared bus)
   - Data: Bus voltage (V), shunt voltage (mV)
   - Resolution: 16-bit
   - Addresses: 4 unique I2C addresses

2. **Processing**:
   - Voltage calculation
   - Power calculation (V × I)
   - Threshold checking
   - Fault detection

3. **Storage**:
   - Latest readings: 4 × 4 bytes = 16 bytes
   - Min/Max values: 32 bytes

4. **Consumers**:
   - Telemetry module
   - Fault handler
   - OBC (via CAN)

##### Current Sensors (4x INA226)
**Flow**: INA226 → I2C → MCU → Monitoring → Telemetry

1. **Acquisition**:
   - Rate: 1 Hz nominal, 100 Hz during actuation
   - Interface: I2C (shared bus)
   - Data: Shunt voltage → current calculation
   - Resolution: 16-bit

2. **Processing**:
   - Current calculation (I = V_shunt / R_shunt)
   - Current feedback for magnetorquer control
   - Over-current detection
   - Power calculation

3. **Storage**:
   - Latest readings: 16 bytes
   - Peak values: 16 bytes

4. **Consumers**:
   - Magnetorquer driver (closed-loop current control)
   - Telemetry module
   - Fault handler

##### Temperature Sensors (4x TMP100)
**Flow**: TMP100 → I2C → MCU → Monitoring → Telemetry

1. **Acquisition**:
   - Rate: 1 Hz (slow monitoring)
   - Interface: I2C (shared bus)
   - Data: Temperature (°C)
   - Resolution: 12-bit (0.0625°C)
   - Addresses: 4 unique I2C addresses

2. **Processing**:
   - Temperature conversion
   - Threshold checking
   - Trend analysis (heating/cooling rate)

3. **Storage**:
   - Latest readings: 4 × 2 bytes = 8 bytes
   - Min/Max values: 16 bytes

4. **Consumers**:
   - Telemetry module
   - Thermal management
   - Fault handler

### 2. Control Data Path

#### 2.1 Attitude Estimation Data Path
**Flow**: Sensor Data → Fusion Algorithm → Attitude Quaternion

1. **Inputs**:
   - Gyroscope angular rates (ωx, ωy, ωz) @ 100 Hz
   - Magnetometer field vector (Bx, By, Bz) @ 100 Hz
   - Sun sensor vector (optional) @ 10 Hz

2. **Processing**:
   - Quaternion propagation from gyroscope
   - Measurement update from magnetometer
   - Covariance update (if EKF)
   - Bias estimation

3. **Output**:
   - Attitude quaternion (q0, q1, q2, q3)
   - Covariance matrix (optional)
   - Rate: 100 Hz
   - Size: 16 bytes (quaternion) + optional covariance

4. **Consumers**:
   - Pointing controller
   - Telemetry module

#### 2.2 Detumbling Control Data Path
**Flow**: Gyroscope + Magnetometer → B-dot Algorithm → Torque Commands

1. **Inputs**:
   - Angular rates (ωx, ωy, ωz)
   - Magnetic field (Bx, By, Bz)
   - Magnetic field derivative (dB/dt)

2. **Processing**:
   - Compute dB/dt (numerical derivative)
   - Apply B-dot control law: M = -k × (dB/dt)
   - Gain scheduling based on angular velocity
   - Saturation limits

3. **Output**:
   - Commanded magnetic dipole moments (Mx, My, Mz)
   - Rate: 10 Hz
   - Size: 12 bytes (3 floats)

4. **Consumers**:
   - Magnetorquer driver module

#### 2.3 Pointing Control Data Path
**Flow**: Attitude Estimate + Target → PID Controller → Torque Commands

1. **Inputs**:
   - Current attitude quaternion
   - Target attitude quaternion
   - Angular rates (ωx, ωy, ωz)
   - Previous errors (for integral term)

2. **Processing**:
   - Compute attitude error (quaternion difference)
   - Convert to Euler angles or axis-angle
   - Apply PID control per axis
   - Anti-windup for integral term
   - Cross-coupling compensation

3. **Output**:
   - Commanded magnetic dipole moments (Mx, My, Mz)
   - Rate: 10 Hz
   - Size: 12 bytes (3 floats)

4. **Consumers**:
   - Magnetorquer driver module

### 3. Actuator Data Path

#### 3.1 Magnetorquer Control Data Path
**Flow**: Torque Commands → PWM Generation → H-Bridge → Magnetorquers

1. **Inputs**:
   - Commanded magnetic dipoles (Mx, My, Mz)
   - Current feedback from INA226 sensors

2. **Processing**:
   - Convert dipole moment to current setpoint
   - Direction determination (sign of M)
   - PWM duty cycle calculation
   - Closed-loop current control
   - Frequency: PWM @ 20 kHz, Control @ 100 Hz

3. **Hardware Control**:
   - Timer PWM output (3 channels)
   - H-bridge direction pins (6 GPIO)
   - H-bridge enable pins (3 GPIO)

4. **Output**:
   - PWM signals to H-bridges (DRV8834PWP)
   - Direction signals (forward/reverse)
   - Current monitoring feedback loop

5. **Feedback**:
   - Actual current measured via INA226
   - Compare to setpoint
   - Adjust PWM duty cycle
   - Update rate: 100 Hz

### 4. Communication Data Path

#### 4.1 CAN Bus TX Data Path
**Flow**: Telemetry/Data → CAN Frame → TCAN330GD → OBC

1. **Inputs**:
   - Telemetry data structure
   - Housekeeping data
   - Attitude estimates
   - Status flags

2. **Processing**:
   - Data packetization
   - CAN ID assignment
   - DLC (Data Length Code) setting
   - Priority assignment
   - Frame formatting

3. **Transmission**:
   - CAN controller buffer load
   - Interrupt-driven transmission
   - ACK confirmation
   - Retry on failure

4. **Rate**: 1 Hz nominal, event-driven for commands

#### 4.2 CAN Bus RX Data Path
**Flow**: OBC → TCAN330GD → CAN Frame → Command Parser

1. **Reception**:
   - Interrupt-driven CAN RX
   - Frame filtering by ID
   - DLC validation
   - Data extraction

2. **Processing**:
   - Command ID parsing
   - Parameter extraction
   - Validation checks
   - CRC verification (if implemented)

3. **Output**:
   - Command structure to handler
   - Event flags to RTOS
   - Execution trigger

4. **Commands**:
   - Mode change requests
   - Configuration updates
   - Calibration commands
   - Telemetry requests

### 5. Telemetry Data Path

#### 5.1 Telemetry Collection
**Flow**: All Subsystems → Telemetry Buffer → CAN TX

1. **Data Sources**:
   - Sensor readings (gyro, mag, sun, housekeeping)
   - Attitude estimates
   - Control outputs
   - System status
   - Error logs

2. **Collection**:
   - Periodic sampling @ 1 Hz
   - Data structure population
   - Timestamp insertion
   - Sequence number increment

3. **Buffer**:
   - Circular buffer for history
   - Size: ~1 KB
   - Depth: ~10 frames

4. **Output**:
   - CAN bus transmission
   - Logging to RAM (optional)

### 6. Watchdog Data Path

**Flow**: Task Health → Watchdog Manager → Watchdog Kick

1. **Monitoring**:
   - Task execution flags
   - Loop counters
   - Timing checks

2. **Processing**:
   - Verify all tasks are running
   - Check for stuck loops
   - Validate timing constraints

3. **Action**:
   - If healthy: Kick watchdog (GPIO toggle)
   - If fault: Allow watchdog timeout → reset
   - Rate: 10 Hz (every 100 ms, timeout = 1600 ms)

## Data Rates and Bandwidth

### I2C Bus Utilization
- **Clock Speed**: 400 kHz (Fast Mode)
- **Devices**: 4x INA226 (voltage), 4x INA226 (current), 4x TMP100, 1x Gyro (optional), 1x Mag (optional)
- **Estimated Usage**:
  - Housekeeping @ 1 Hz: ~1% utilization
  - Sensors @ 100 Hz: ~50% utilization (if using I2C)
- **Margin**: Sufficient bandwidth

### SPI Bus Utilization
- **Clock Speed**: Up to 18 MHz (72 MHz / 4)
- **Devices**: Gyroscope, Magnetometer (if not using I2C)
- **Estimated Usage**: ~5% @ 100 Hz sampling
- **Margin**: Ample bandwidth

### CAN Bus Utilization
- **Baud Rate**: 250 kbps or 500 kbps
- **Frame Size**: ~80 bits average (with arbitration)
- **Telemetry**: 1 Hz × 80 bits = 80 bps
- **Commands**: Event-driven, <10 bps average
- **Total Usage**: <1% utilization
- **Margin**: Very high

### CPU Loading Estimate
- **Sensor Reading**: 10% (100 Hz task)
- **Control Algorithms**: 20% (10 Hz task with floating-point math)
- **Communication**: 5% (CAN, I2C overhead)
- **Telemetry**: 5% (1 Hz task)
- **Idle/Margin**: 60%

## Data Storage Requirements

### RAM Requirements
- **Sensor Data**: 200 bytes (current readings + buffers)
- **Attitude State**: 64 bytes (quaternion, covariance)
- **Control Variables**: 128 bytes (setpoints, errors, outputs)
- **Telemetry Buffer**: 1 KB
- **Command Buffer**: 256 bytes
- **RTOS Overhead**: 8 KB
- **Total**: ~10 KB (of 40 KB available)

### Flash Requirements
- **Firmware Code**: ~100 KB
- **Calibration Data**: 256 bytes
- **Configuration**: 1 KB
- **Total**: ~102 KB (of 256 KB available)

## Data Integrity and Safety

### Error Detection
- **I2C**: ACK/NACK checking
- **SPI**: CRC or parity (if supported by sensor)
- **CAN**: Built-in CRC-15
- **Sensor Data**: Range checking, validity flags

### Fault Handling
- **Invalid Data**: Reject and retry
- **Communication Timeout**: Use last valid data, flag error
- **Out-of-Range**: Clamp to limits, trigger fault
- **Watchdog**: System reset on critical failure

### Data Consistency
- **Atomic Reads**: Disable interrupts during multi-byte reads
- **Mutex Protection**: RTOS mutex for shared resources
- **Data Versioning**: Sequence numbers for telemetry frames
