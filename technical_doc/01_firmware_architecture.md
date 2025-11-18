# Firmware Architecture

## Overview
The ADCS firmware is designed for the STM32F303RCT6 microcontroller (ARM Cortex-M4F) and implements a real-time control system for satellite attitude determination and control.

## Microcontroller Specifications

### STM32F303RCT6
- **Core**: ARM Cortex-M4F with FPU (Floating Point Unit)
- **Clock Speed**: Up to 72 MHz
- **Flash Memory**: 256 KB
- **RAM**: 40 KB
- **Package**: LQFP64
- **Operating Voltage**: 2.0V to 3.6V
- **Key Features**:
  - Hardware FPU for floating-point calculations (critical for control algorithms)
  - Multiple I2C, SPI, UART interfaces
  - 12-bit ADC
  - Timers for PWM generation
  - CAN bus interface
  - Low-power modes

## Firmware Architecture Layers

```
┌─────────────────────────────────────────────────┐
│         Application Layer                       │
│  - Control Algorithms (Detumbling, Pointing)   │
│  - Mission Logic                                │
│  - Command Handler                              │
└─────────────────────────────────────────────────┘
                    ↕
┌─────────────────────────────────────────────────┐
│         Control & Estimation Layer              │
│  - Attitude Estimation (Sensor Fusion)         │
│  - PID Controllers                              │
│  - B-dot Detumbling Control                     │
│  - Kalman Filters (optional)                    │
└─────────────────────────────────────────────────┘
                    ↕
┌─────────────────────────────────────────────────┐
│         Hardware Abstraction Layer (HAL)        │
│  - Sensor Drivers (Gyro, Mag, Temp, Volt)     │
│  - Actuator Drivers (Magnetorquer PWM)         │
│  - Communication Drivers (CAN, I2C, SPI)       │
│  - Watchdog Driver                              │
└─────────────────────────────────────────────────┘
                    ↕
┌─────────────────────────────────────────────────┐
│         RTOS / Scheduler Layer                  │
│  - Task Scheduling                              │
│  - Inter-task Communication                     │
│  - Timing and Synchronization                   │
└─────────────────────────────────────────────────┘
                    ↕
┌─────────────────────────────────────────────────┐
│         Hardware (STM32 Peripherals)            │
│  - GPIO, I2C, SPI, CAN, Timers, ADC            │
└─────────────────────────────────────────────────┘
```

## Software Modules

### 1. Initialization Module
- **Function**: System startup and configuration
- **Responsibilities**:
  - Clock configuration (PLL setup for 72 MHz)
  - Peripheral initialization (I2C, SPI, CAN, Timers)
  - GPIO configuration
  - Watchdog initialization
  - RTOS startup
  - Self-test procedures

### 2. Sensor Drivers

#### 2.1 Gyroscope Driver (I3G4250DTR)
- **Interface**: SPI or I2C
- **Functions**:
  - Initialization and configuration
  - Angular rate reading (3-axis)
  - Temperature compensation
  - Calibration routines
  - Error detection

#### 2.2 Magnetometer Driver (MMC5983MA)
- **Interface**: SPI or I2C
- **Functions**:
  - Initialization and configuration
  - Magnetic field vector reading (3-axis)
  - Calibration (hard iron, soft iron)
  - Noise filtering
  - Self-test execution

#### 2.3 Sun Sensor Driver
- **Interface**: TBD
- **Functions**: TBD

#### 2.4 Housekeeping Sensor Drivers
- **INA226 Driver** (Voltage/Current monitoring)
  - I2C interface
  - Voltage reading
  - Current reading
  - Power calculation
  - Alert configuration

- **TMP100 Driver** (Temperature monitoring)
  - I2C interface
  - Temperature reading
  - Alert threshold configuration

### 3. Actuator Drivers

#### 3.1 Magnetorquer Driver
- **Interface**: PWM (Timer-based)
- **H-Bridge Control**: DRV8834PWP driver IC
- **Functions**:
  - PWM signal generation (per axis)
  - Current setpoint control
  - Direction control (forward/reverse)
  - Current feedback loop
  - Fault detection and protection
  - Enable/disable control

### 4. Communication Modules

#### 4.1 CAN Bus Driver
- **Transceiver**: TCAN330GD
- **Functions**:
  - CAN initialization and configuration
  - Message transmission
  - Message reception with filtering
  - Error handling
  - Frame formatting (command, telemetry)

#### 4.2 I2C Master Driver
- **Purpose**: Sensor communication bus
- **Functions**:
  - Multi-device support
  - Read/write transactions
  - Error recovery
  - Bus arbitration

#### 4.3 SPI Master Driver
- **Purpose**: High-speed sensor communication
- **Functions**:
  - Device selection (chip select)
  - Data transfer
  - Clock configuration

### 5. Control Algorithms

#### 5.1 Detumbling Controller
- **Algorithm**: B-dot control law
- **Inputs**:
  - Angular rates from gyroscope
  - Magnetic field from magnetometer
- **Outputs**: Commanded magnetic dipole moments
- **Implementation**:
  - Derivative calculation of magnetic field
  - Gain scheduling
  - Torque command generation

#### 5.2 Pointing Controller
- **Algorithm**: PID control (3-axis)
- **Inputs**:
  - Estimated attitude
  - Target attitude
  - Angular rates
- **Outputs**: Commanded magnetic dipole moments
- **Implementation**:
  - Error calculation (quaternion or Euler angles)
  - PID computation per axis
  - Anti-windup protection
  - Gain scheduling

#### 5.3 Attitude Estimator
- **Algorithm**: Complementary filter or Extended Kalman Filter (EKF)
- **Inputs**:
  - Gyroscope angular rates
  - Magnetometer field vector
  - Sun sensor vector (optional)
- **Output**: Estimated attitude (quaternion)
- **Implementation**:
  - Sensor fusion
  - Bias estimation
  - Covariance propagation (if EKF)

### 6. Telemetry and Data Handling

#### 6.1 Telemetry Module
- **Function**: Collect and format system telemetry
- **Data Collected**:
  - Sensor readings (gyro, magnetometer, sun sensor)
  - Housekeeping data (voltage, current, temperature)
  - Attitude estimates
  - Control outputs
  - System status and health
  - Error logs

#### 6.2 Data Logging
- **Function**: Store critical data for diagnostics
- **Storage**: RAM buffer (circular buffer)
- **Data**: High-rate sensor data, events, faults

### 7. Command Handler
- **Function**: Process commands from OBC
- **Commands**:
  - Mode change (Idle, Detumbling, Pointing)
  - Configuration updates
  - Calibration commands
  - Telemetry requests
  - Reset commands
- **Protocol**: CAN bus messaging

### 8. Health Monitoring and Fault Management

#### 8.1 Watchdog Manager
- **External Watchdog**: TPS3823 (hardware)
- **Function**: Periodic watchdog tickling
- **Timeout**: 1600 ms
- **Action**: Reset on failure to service

#### 8.2 Fault Detection
- **Monitored Faults**:
  - Sensor communication failures
  - Over-current conditions
  - Over-temperature conditions
  - Invalid sensor data
  - Actuator faults
- **Actions**:
  - Fault logging
  - Safe mode transition
  - Alert to OBC
  - Automatic recovery attempts

### 9. Mode Manager
- **Function**: State machine for operating modes
- **States**:
  - **INIT**: Initialization and self-test
  - **IDLE**: Low-power standby
  - **DETUMBLING**: Active detumbling control
  - **POINTING**: Active pointing control
  - **SAFE**: Fault-safe mode
- **Transitions**: Based on commands or fault conditions

## Memory Map

### Flash Memory Allocation (256 KB)
- **Bootloader**: 0x0800 0000 - 0x0800 3FFF (16 KB)
- **Application**: 0x0800 4000 - 0x0803 FFFF (240 KB)
- **Reserved**: Last sector for configuration storage

### RAM Allocation (40 KB)
- **Stack**: 4 KB
- **Heap**: 8 KB
- **RTOS**: 8 KB (task stacks, queues, semaphores)
- **Buffers**: 16 KB (sensor data, telemetry, logs)
- **Global Variables**: 4 KB

## Timing and Real-Time Constraints

### Task Execution Rates
- **Sensor Reading Task**: 100 Hz (10 ms period)
- **Control Update Task**: 10 Hz (100 ms period)
- **Telemetry Task**: 1 Hz (1 s period)
- **Watchdog Task**: 10 Hz (100 ms period)
- **CAN Communication**: Event-driven + 1 Hz heartbeat

### Interrupt Priorities
1. **Highest**: Hardware faults (NMI)
2. **High**: Watchdog timer
3. **Medium**: CAN bus interrupts
4. **Low**: I2C/SPI completion interrupts
5. **Lowest**: Systick (RTOS scheduler)

## Development Tools and Framework

### Toolchain
- **Compiler**: GCC ARM Embedded
- **IDE**: STM32CubeIDE or VS Code
- **HAL**: STM32Cube HAL/LL libraries
- **RTOS**: FreeRTOS (optional but recommended)
- **Debugger**: ST-Link / J-Link

### Build System
- **Make**: Makefile-based build
- **Version Control**: Git

### Testing
- **Unit Tests**: Host-based testing with mocks
- **Hardware-in-Loop (HIL)**: Integration testing with actual hardware
- **Simulation**: Software-in-Loop (SIL) for control algorithms

## Configuration Management
- **Compile-time Configuration**: Header files with defines
- **Runtime Configuration**: Parameters stored in flash (last sector)
- **Calibration Data**: Stored in non-volatile memory

## Boot Sequence

1. **Reset**: Hardware reset or watchdog timeout
2. **System Clock Init**: Configure PLL for 72 MHz
3. **Peripheral Init**: Initialize all peripherals
4. **Watchdog Init**: Start external watchdog
5. **Self-Test**: Sensor communication checks
6. **RTOS Start**: Launch task scheduler (if used)
7. **Idle Mode**: Wait for commands from OBC
8. **Mode Transition**: Execute commanded mode

## Future Enhancements
- Bootloader for in-orbit firmware updates
- Enhanced sensor fusion algorithms
- Adaptive control gains
- Extended telemetry and diagnostics
- Flash data logging for mission replay
