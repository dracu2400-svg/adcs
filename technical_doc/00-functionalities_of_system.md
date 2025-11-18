# System Functionalities

## Overview
The ADCS (Attitude Determination and Control System) is designed for 3U CubeSat missions, providing three-axis attitude determination and control capabilities using magnetic actuators.

## Core Functionalities

### 1. Attitude Determination
- **3-Axis Gyroscope Sensing**: Real-time angular rate measurements using I3G4250DTR MEMS gyroscope
- **3-Axis Magnetometer Sensing**: Earth's magnetic field measurement using MMC5983MA magnetometer
- **Sun Sensor Integration**: Solar vector determination for attitude reference (TBD)
- **Sensor Fusion**: Combined sensor data processing for accurate attitude estimation

### 2. Attitude Control

#### 2.1 Detumbling Mode
- **Function**: Reduce satellite angular velocity after deployment or anomalies
- **Method**: B-dot control law using magnetorquers
- **Input**: Gyroscope angular rate measurements
- **Output**: Commanded magnetic dipole moments to magnetorquers
- **Goal**: Stabilize satellite rotation to safe operating rates

#### 2.2 Pointing Mode
- **Function**: 3-axis attitude control for mission objectives
- **Method**: PID control with magnetorquer actuation
- **Capabilities**:
  - Nadir pointing for Earth observation
  - Solar panel sun tracking
  - Antenna pointing for communications
  - Instrument pointing for payloads
- **Target Performance**: TBD pointing accuracy

#### 2.3 Idle Mode
- **Function**: Minimal power consumption when attitude control is not required
- **Behavior**: Sensors in low-power mode, actuators disabled
- **Monitoring**: Passive telemetry collection only

### 3. Magnetic Actuation
- **X-Axis Magnetorquer**: Soft magnetic core, magnetic dipole moment ≥0.2 Am²
- **Y-Axis Magnetorquer**: Soft magnetic core, magnetic dipole moment ≥0.2 Am²
- **Z-Axis Magnetorquer**: Air-core design, magnetic dipole moment ≥0.2 Am²
- **Drive Method**: PWM-controlled H-bridge drivers (DRV8834PWP)
- **Control**: Closed-loop current monitoring and regulation

### 4. Telemetry and Monitoring

#### 4.1 Voltage Monitoring
- **Sensors**: 4x INA226 high-side current/voltage monitors
- **Interface**: I2C bus
- **Purpose**: Power supply monitoring and fault detection

#### 4.2 Current Monitoring
- **Sensors**: 4x INA226 integrated shunt and power monitors
- **Interface**: I2C bus
- **Purpose**: Magnetorquer current measurement and power consumption tracking

#### 4.3 Temperature Monitoring
- **Sensors**: 4x TMP100 temperature sensors
- **Interface**: I2C bus
- **Purpose**: Thermal management and component protection
- **Distribution**: Critical components and board zones

### 5. Communication and Interface

#### 5.1 CAN Bus Communication
- **Transceiver**: TCAN330GD CAN transceiver
- **Purpose**: Primary communication with satellite OBC (On-Board Computer)
- **Data**: Commands, telemetry, mode changes, attitude data

#### 5.2 PC-104 Mechanical Interface
- **Form Factor**: PC-104 compatible
- **Dimensions**: 90mm x 90mm (fits 3U CubeSat)
- **Connector**: Stackable bus interface
- **Integration**: Plug-and-play with CubeSat stack

### 6. Fault Detection and Recovery

#### 6.1 External Watchdog
- **IC**: Texas Instruments TPS3823
- **Timeout**: 1600 ms
- **Function**: MCU health monitoring
- **Action**: Automatic system reset on watchdog timeout

#### 6.2 Over-Current Protection
- **Method**: Current monitoring via INA226 sensors
- **Action**: Magnetorquer shutdown on fault detection

#### 6.3 Over-Temperature Protection
- **Method**: Temperature monitoring via TMP100 sensors
- **Thresholds**: TBD based on component specifications
- **Action**: System de-rating or shutdown

### 7. Power Management
- **Input Voltage**: Up to 12V from satellite power bus
- **Power Distribution**: Regulated supplies for MCU, sensors, and actuators
- **Power Modes**: Active, low-power, and idle modes
- **Current Budget**: TBD based on mission profile

## Operating Modes Summary

| Mode | Sensors Active | Actuators Active | Power Consumption | Purpose |
|------|----------------|------------------|-------------------|---------|
| Detumbling | Gyroscope, Magnetometer | Magnetorquers (PWM) | High | Initial stabilization |
| Pointing | All sensors | Magnetorquers (PWM) | Medium | Mission operations |
| Idle | Temperature only | None | Low | Safe mode / power saving |

## Performance Targets
- **Pointing Accuracy**: TBD
- **Detumbling Time**: TBD
- **Magnetic Dipole Moment**: ≥0.2 Am² per axis
- **Operating Temperature Range**: TBD
- **Power Consumption**: TBD per mode
