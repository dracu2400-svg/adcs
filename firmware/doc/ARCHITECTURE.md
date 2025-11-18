# ADCS Firmware Architecture

**Document Version:** 1.0
**Target System:** STM32L476RG-based ADCS
**Last Updated:** 2025-11-18

## Table of Contents

1. [System Overview](#system-overview)
2. [Firmware Architecture](#firmware-architecture)
3. [Data Flow Architecture](#data-flow-architecture)
4. [Task Architecture](#task-architecture)
5. [Module Design](#module-design)
6. [Communication Architecture](#communication-architecture)
7. [Control Loop Design](#control-loop-design)
8. [Memory Architecture](#memory-architecture)
9. [State Management](#state-management)
10. [Error Handling](#error-handling)

---

## System Overview

### Purpose

The ADCS (Attitude Determination and Control System) is responsible for:
- **Determining** the spacecraft's orientation using sensors
- **Controlling** the spacecraft's attitude using magnetorquers
- **Communicating** telemetry and receiving commands via CAN bus
- **Monitoring** system health (current, temperature, faults)

### Key Components

```
┌─────────────────────────────────────────────────────────────────┐
│                        ADCS System                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐    │
│  │   Sensors    │───▶│ STM32L476RG  │───▶│  Actuators   │    │
│  │              │    │              │    │              │    │
│  │ • Magnetom.  │    │ • Attitude   │    │ • 3-Axis     │    │
│  │ • Gyroscope  │    │   Estimation │    │   Magneto-   │    │
│  │ • Sun Sens.  │    │ • Control    │    │   rquers     │    │
│  │ • Current    │    │   Algorithm  │    │              │    │
│  │ • Temp       │    │ • Health Mon.│    │              │    │
│  └──────────────┘    └──────────────┘    └──────────────┘    │
│         ▲                    ▲                                 │
│         │                    │                                 │
│         │            ┌───────▼───────┐                        │
│         │            │   CAN Bus     │                        │
│         │            │  Interface    │                        │
│         │            │ (PC-104 Bus)  │                        │
│         │            └───────────────┘                        │
│         │                    ▲                                 │
│         │                    │                                 │
│         └────────────────────┴─────────────────────────────────┤
│                     External Systems                           │
│              (OBC, Telemetry, Ground Control)                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## Firmware Architecture

### Architecture Pattern

The firmware follows a **Layered Real-Time Architecture** with FreeRTOS:

```
┌─────────────────────────────────────────────────────────────────┐
│                    Application Layer                           │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐        │
│  │  Attitude    │  │   Health     │  │  Command     │        │
│  │  Control     │  │  Monitoring  │  │  Handling    │        │
│  └──────────────┘  └──────────────┘  └──────────────┘        │
├─────────────────────────────────────────────────────────────────┤
│                     Service Layer                              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐        │
│  │  Sensor      │  │  Actuator    │  │   Protocol   │        │
│  │  Drivers     │  │  Drivers     │  │   Handlers   │        │
│  └──────────────┘  └──────────────┘  └──────────────┘        │
├─────────────────────────────────────────────────────────────────┤
│                      HAL Layer                                 │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐        │
│  │   I2C/SPI    │  │   PWM/GPIO   │  │     CAN      │        │
│  │     HAL      │  │     HAL      │  │     HAL      │        │
│  └──────────────┘  └──────────────┘  └──────────────┘        │
├─────────────────────────────────────────────────────────────────┤
│                  FreeRTOS Kernel                               │
│  (Task Scheduling, IPC, Synchronization, Timers)               │
├─────────────────────────────────────────────────────────────────┤
│                  Hardware (STM32L476RG)                        │
└─────────────────────────────────────────────────────────────────┘
```

### Directory Structure by Layer

```
firmware/
├── App/                        # Application Layer
│   ├── attitude_control/       # Attitude control algorithms
│   ├── health_monitor/         # Health monitoring
│   └── command_handler/        # Command processing
├── Drivers/                    # Service Layer (Custom Drivers)
│   ├── sensors/                # Sensor drivers (INA226, TMP100, IMU)
│   ├── actuators/              # Magnetorquer driver
│   └── protocol/               # CAN protocol implementation
├── Core/                       # HAL Layer (STM32 HAL wrapper)
│   ├── Src/                    # Peripheral initialization
│   └── Inc/                    # Hardware configuration
└── Middlewares/FreeRTOS/       # RTOS Kernel
```

---

## Data Flow Architecture

### High-Level Data Flow

```
┌───────────────────────────────────────────────────────────────────────┐
│                         ADCS Data Flow                                │
└───────────────────────────────────────────────────────────────────────┘

INPUT SENSORS                  PROCESSING                    OUTPUT ACTUATORS
─────────────                  ──────────                    ────────────────

┌─────────────┐               ┌─────────────┐              ┌─────────────┐
│ Magnetom.   │──I2C/SPI─────▶│  Attitude   │              │  Coil 0     │
│ (3-axis)    │   100Hz       │ Estimation  │              │  (X-axis)   │
└─────────────┘               │             │              └─────────────┘
                              │  • Kalman   │                     ▲
┌─────────────┐               │    Filter   │                     │
│ Gyroscope   │──I2C/SPI─────▶│  • Sensor   │              ┌─────┴─────┐
│ (3-axis)    │   100Hz       │    Fusion   │              │   PWM     │
└─────────────┘               │             │              │ Generator │
                              └──────┬──────┘              │  20 kHz   │
┌─────────────┐                      │                     └─────┬─────┘
│ Sun Sensors │──ADC─────────────────┤                           │
│ (4 units)   │   100Hz              │                           │
└─────────────┘                      │                           │
                              ┌──────▼──────┐              ┌─────▼─────┐
┌─────────────┐               │  Attitude   │              │  Coil 1   │
│ INA226 (4x) │──I2C─────────▶│   Control   │──100Hz──────▶│  (Y-axis) │
│ Current Mon.│   50Hz        │             │              └───────────┘
└─────────────┘               │  • B-dot    │
                              │  • PID      │              ┌───────────┐
┌─────────────┐               │  • Detumb.  │              │  Coil 2   │
│ TMP100 (4x) │──I2C─────────▶│             │──100Hz──────▶│  (Z-axis) │
│ Temp. Mon.  │   10Hz        └──────┬──────┘              └───────────┘
└─────────────┘                      │
                                     │
                              ┌──────▼──────┐              ┌───────────┐
┌─────────────┐               │  Telemetry  │              │           │
│  External   │◀──CAN────────▶│   & CAN     │──50Hz───────▶│  CAN TX   │
│  Commands   │   50Hz        │  Protocol   │              │           │
└─────────────┘               └─────────────┘              └───────────┘
```

### Detailed Data Path

#### 1. Sensor Data Acquisition Path

```
Physical Signal → I2C/SPI/ADC → HAL Driver → Sensor Driver → Data Queue
                   Hardware      STM32 HAL    Custom Code    FreeRTOS
```

**Flow:**
1. **Hardware Interface**: I2C/SPI/ADC peripheral samples sensor
2. **HAL Driver**: STM32 HAL provides low-level read/write
3. **Sensor Driver**: Custom driver formats data, applies calibration
4. **Data Queue**: FreeRTOS queue transfers data to processing task
5. **Processing Task**: Attitude estimation task consumes data

**Example - Magnetometer Reading:**
```c
// 1. HAL reads raw I2C data
HAL_I2C_Mem_Read(&hi2c1, MAG_ADDR, MAG_DATA_REG, raw_data, 6);

// 2. Sensor driver converts to engineering units
magnetometer_convert(raw_data, &mag_data);  // Output: µT

// 3. Apply calibration
magnetometer_calibrate(&mag_data, &calib_params);

// 4. Send to queue
xQueueSend(mag_queue, &mag_data, 0);

// 5. Attitude task receives
xQueueReceive(mag_queue, &mag_reading, timeout);
```

#### 2. Control Output Path

```
Control Algorithm → Actuator Driver → PWM HAL → H-Bridge → Coil Current
  (Duty Cycle)      (Direction/Enable)  Timer    Hardware   Physical
```

**Flow:**
1. **Control Algorithm**: Computes desired magnetic dipole moment
2. **Dipole → Current**: Converts dipole to coil current (Amperes)
3. **Current → PWM**: Maps current to PWM duty cycle (0-100%)
4. **Actuator Driver**: Sets direction and enable pins
5. **PWM HAL**: Configures timer CCR register
6. **H-Bridge**: Switches current through coil

**Example - Magnetorquer Control:**
```c
// 1. Control algorithm output (in A·m²)
float dipole_moment[3] = {0.05, -0.03, 0.02};  // Desired dipole

// 2. Convert to current (I = dipole / (N * A))
float coil_current[3];
for (int i = 0; i < 3; i++) {
    coil_current[i] = dipole_moment[i] / (COIL_TURNS * COIL_AREA);
}

// 3. Current limiting
for (int i = 0; i < 3; i++) {
    coil_current[i] = CLAMP(coil_current[i], -MAX_CURRENT, MAX_CURRENT);
}

// 4. Convert to PWM duty cycle
uint16_t pwm_duty[3];
for (int i = 0; i < 3; i++) {
    pwm_duty[i] = (uint16_t)(fabs(coil_current[i]) / MAX_CURRENT * PWM_ARR_VALUE);
}

// 5. Set direction and enable
magnetorquer_set_coil(0, coil_current[0] >= 0, pwm_duty[0], true);
magnetorquer_set_coil(1, coil_current[1] >= 0, pwm_duty[1], true);
magnetorquer_set_coil(2, coil_current[2] >= 0, pwm_duty[2], true);
```

#### 3. Communication Data Path

```
CAN RX → HAL ISR → CAN Driver → Command Queue → Handler Task → Response
                                                                     ↓
CAN TX ← HAL      ← CAN Driver ← Telemetry Queue ← Processing ←─────┘
```

**Flow:**
- **RX Path**: CAN interrupt → Parse command → Queue to handler → Execute → Queue response
- **TX Path**: Telemetry data → Format message → Queue to CAN task → Transmit

---

## Task Architecture

### FreeRTOS Task Structure

```
┌─────────────────────────────────────────────────────────────────┐
│                      Task Hierarchy                             │
└─────────────────────────────────────────────────────────────────┘

Priority 4 (HIGHEST)    ┌─────────────────────────────┐
                        │    CAN RX ISR Handler       │
                        │    (Interrupt Context)      │
                        └──────────────┬──────────────┘
                                       │
Priority 3 (HIGH)       ┌──────────────▼──────────────┐
                        │  Attitude Control Task      │
                        │  Period: 10ms (100 Hz)      │
                        │  Stack: 512 words           │
                        └──────────────┬──────────────┘
                                       │
Priority 2 (MEDIUM)     ┌──────────────▼──────────────┐
                        │  Sensor Read Task           │
                        │  Period: 100ms              │
                        │  Stack: 256 words           │
                        └──────────────┬──────────────┘
                                       │
Priority 2 (MEDIUM)     ┌──────────────▼──────────────┐
                        │  CAN Communication Task     │
                        │  Period: 50ms               │
                        │  Stack: 256 words           │
                        └──────────────┬──────────────┘
                                       │
Priority 1 (LOW)        ┌──────────────▼──────────────┐
                        │  Health Monitor Task        │
                        │  Period: 500ms              │
                        │  Stack: 256 words           │
                        └──────────────┬──────────────┘
                                       │
Priority 1 (LOW)        ┌──────────────▼──────────────┐
                        │  Watchdog Task              │
                        │  Period: 100ms              │
                        │  Stack: 128 words           │
                        └─────────────────────────────┘

Priority 0              Idle Task (FreeRTOS built-in)
```

### Task Timing Diagram

```
Time (ms)  0    10   20   30   40   50   60   70   80   90   100  110  120
           │    │    │    │    │    │    │    │    │    │    │    │    │
AttCtrl    ▓▓   ▓▓   ▓▓   ▓▓   ▓▓   ▓▓   ▓▓   ▓▓   ▓▓   ▓▓   ▓▓   ▓▓   (10ms)
           │    │    │    │    │    │    │    │    │    │    │    │
CANComm    ▓▓▓▓           ▓▓▓▓           ▓▓▓▓           ▓▓▓▓            (50ms)
           │    │    │    │    │    │    │    │    │    │    │    │
SensorRd   ▓▓▓▓▓▓▓▓▓                    ▓▓▓▓▓▓▓▓▓                      (100ms)
           │    │    │    │    │    │    │    │    │    │    │    │
HealthMon  ▓▓▓                                   ▓▓▓                   (500ms)
           │    │    │    │    │    │    │    │    │    │    │    │
Watchdog   ▓    │    │    │    │    │    │    │    │    ▓    │    │   (100ms)
           │    │    │    │    │    │    │    │    │    │    │    │

Legend:  ▓ = Task executing    │ = Time tick (10ms)
```

### Task Details

#### Task 1: Attitude Control (100 Hz)

**Purpose**: Main control loop for spacecraft attitude
**Priority**: 3 (High)
**Period**: 10 ms
**WCET**: ~5 ms

**Algorithm:**
```
Loop forever:
  1. Wait for timer semaphore (10ms period)
  2. Read latest sensor data from queues
  3. Run attitude estimation (Kalman filter)
  4. Run control algorithm (B-dot / PID)
  5. Compute magnetorquer commands
  6. Send commands to actuator driver
  7. Log telemetry
```

**Inputs:**
- Magnetometer data (3-axis, µT)
- Gyroscope data (3-axis, rad/s)
- Sun sensor data (4 channels, V)
- Target attitude (from command queue)

**Outputs:**
- Magnetorquer PWM commands (3-axis, duty cycle)
- Estimated attitude quaternion
- Control error metrics

#### Task 2: Sensor Read (10 Hz)

**Purpose**: Acquire data from all sensors
**Priority**: 2 (Medium)
**Period**: 100 ms
**WCET**: ~30 ms

**Algorithm:**
```
Loop forever:
  1. Wait for timer (100ms period)
  2. Read magnetometer via I2C/SPI
  3. Read gyroscope via I2C/SPI
  4. Read sun sensors via ADC
  5. Read current sensors (INA226) via I2C
  6. Read temperature sensors (TMP100) via I2C
  7. Apply calibration
  8. Send to queues
```

#### Task 3: CAN Communication (20 Hz)

**Purpose**: Handle CAN bus communication
**Priority**: 2 (Medium)
**Period**: 50 ms
**WCET**: ~10 ms

**Algorithm:**
```
Loop forever:
  1. Wait for timer (50ms period)
  2. Check for received commands (non-blocking)
  3. Process commands if available
  4. Prepare telemetry packet
  5. Transmit telemetry via CAN
  6. Handle acknowledgments
```

#### Task 4: Health Monitor (2 Hz)

**Purpose**: Monitor system health and safety
**Priority**: 1 (Low)
**Period**: 500 ms
**WCET**: ~20 ms

**Algorithm:**
```
Loop forever:
  1. Wait for timer (500ms period)
  2. Check coil currents (overcurrent?)
  3. Check coil temperatures (overtemp?)
  4. Check MCU temperature
  5. Check power supply voltages
  6. Check for fault conditions
  7. Take safety action if needed (disable coils)
  8. Log health data
```

#### Task 5: Watchdog (10 Hz)

**Purpose**: Service external watchdog timer
**Priority**: 1 (Low)
**Period**: 100 ms
**WCET**: <1 ms

**Algorithm:**
```
Loop forever:
  1. Wait for timer (100ms period)
  2. Toggle watchdog GPIO pin
  3. Check task heartbeats (optional)
```

---

## Module Design

### Sensor Modules

#### Magnetometer Driver (MMC5983MA)

**Interface:**
```c
typedef struct {
    float x;      // µT
    float y;      // µT
    float z;      // µT
    uint32_t timestamp;
} mag_data_t;

// Initialize magnetometer
HAL_StatusTypeDef mag_init(void);

// Read magnetometer data
HAL_StatusTypeDef mag_read(mag_data_t *data);

// Apply calibration (hard/soft iron correction)
void mag_calibrate(mag_data_t *raw, mag_data_t *cal, mag_calib_t *params);
```

**Data Flow:**
1. I2C read from MMC5983MA registers
2. Convert 18-bit raw data to µT
3. Apply hard-iron offset correction
4. Apply soft-iron matrix correction
5. Output calibrated 3-axis magnetic field

#### Gyroscope Driver (I3G4250D)

**Interface:**
```c
typedef struct {
    float x;      // rad/s
    float y;      // rad/s
    float z;      // rad/s
    uint32_t timestamp;
} gyro_data_t;

HAL_StatusTypeDef gyro_init(void);
HAL_StatusTypeDef gyro_read(gyro_data_t *data);
void gyro_calibrate(gyro_data_t *raw, gyro_data_t *cal, gyro_calib_t *params);
```

#### Current/Voltage Monitor (INA226)

**Interface:**
```c
typedef struct {
    float voltage;    // Volts
    float current;    // Amperes
    float power;      // Watts
    uint8_t coil_id;  // 0-3
} ina226_data_t;

HAL_StatusTypeDef ina226_init(uint8_t coil_id);
HAL_StatusTypeDef ina226_read(uint8_t coil_id, ina226_data_t *data);
```

**Calibration:**
- Shunt resistor: 0.1Ω (typical)
- Full-scale current: ±1A
- Resolution: ~30µA per LSB

### Actuator Modules

#### Magnetorquer Driver

**Interface:**
```c
typedef struct {
    float current;        // Amperes (-MAX to +MAX)
    uint16_t pwm_duty;    // 0 to PWM_ARR_VALUE
    bool direction;       // false=forward, true=reverse
    bool enabled;         // Coil enable state
} magnetorquer_state_t;

// Initialize all 3 magnetorquers
void magnetorquer_init(void);

// Set coil current (automatic PWM + direction)
void magnetorquer_set_current(uint8_t coil_id, float current);

// Emergency disable all coils
void magnetorquer_disable_all(void);

// Get current state
void magnetorquer_get_state(uint8_t coil_id, magnetorquer_state_t *state);
```

**Current to PWM Conversion:**
```c
void magnetorquer_set_current(uint8_t coil_id, float current) {
    // Clamp to limits
    current = CLAMP(current, -MAX_CURRENT, MAX_CURRENT);

    // Determine direction
    bool direction = (current >= 0);

    // Convert to PWM duty cycle
    float duty_percent = fabs(current) / MAX_CURRENT * 100.0f;
    uint16_t pwm_duty = PWM_DUTY_PERCENT_TO_CCR(duty_percent);

    // Set hardware
    set_direction_pin(coil_id, direction);
    set_pwm_duty(coil_id, pwm_duty);
    enable_coil(coil_id);
}
```

### Control Algorithms

#### B-dot Detumbling Controller

**Purpose**: Dampen angular velocity (initial detumbling)

**Algorithm:**
```c
void bdot_controller(const float B[3], const float B_prev[3],
                     float dt, float dipole_out[3]) {
    // Compute dB/dt (numerical derivative)
    float dB_dt[3];
    for (int i = 0; i < 3; i++) {
        dB_dt[i] = (B[i] - B_prev[i]) / dt;
    }

    // B-dot control law: m = -k * (dB/dt)
    for (int i = 0; i < 3; i++) {
        dipole_out[i] = -K_BDOT * dB_dt[i];
    }
}
```

**Gain Selection:**
- `K_BDOT` typically 1e4 to 1e6 (A·m²·s/T)
- Higher gain = faster detumbling, more power

#### PID Attitude Controller

**Purpose**: Track desired attitude quaternion

**Algorithm:**
```c
typedef struct {
    float kp;     // Proportional gain
    float ki;     // Integral gain
    float kd;     // Derivative gain
    float integral[3];     // Integral error
    float prev_error[3];   // Previous error
} pid_state_t;

void pid_controller(const float q_target[4], const float q_current[4],
                    const float omega[3], float dt,
                    pid_state_t *state, float dipole_out[3]) {
    // Compute attitude error (quaternion error)
    float q_error[4];
    quaternion_error(q_target, q_current, q_error);

    // Convert quaternion error to axis-angle
    float error_axis[3];
    quaternion_to_axis_angle(q_error, error_axis);

    // PID control
    for (int i = 0; i < 3; i++) {
        // Proportional term
        float p_term = state->kp * error_axis[i];

        // Integral term
        state->integral[i] += error_axis[i] * dt;
        float i_term = state->ki * state->integral[i];

        // Derivative term (use gyro for angular velocity)
        float d_term = state->kd * omega[i];

        // Output dipole moment
        dipole_out[i] = p_term + i_term - d_term;
    }
}
```

---

## Communication Architecture

### CAN Protocol Stack

```
┌─────────────────────────────────────────────────────────┐
│                Application Messages                     │
│  (Commands, Telemetry, Configuration)                   │
├─────────────────────────────────────────────────────────┤
│              Message Layer (Framing)                    │
│  • Message ID assignment                                │
│  • Data packing/unpacking                               │
│  • CRC calculation                                      │
├─────────────────────────────────────────────────────────┤
│              Transport Layer                            │
│  • Segmentation (for >8 byte messages)                  │
│  • Acknowledgment handling                              │
├─────────────────────────────────────────────────────────┤
│              CAN Driver                                 │
│  • TX/RX buffer management                              │
│  • Interrupt handling                                   │
├─────────────────────────────────────────────────────────┤
│              STM32 HAL CAN Driver                       │
│  • Low-level CAN peripheral control                     │
└─────────────────────────────────────────────────────────┘
```

### CAN Message Format

**Standard 11-bit ID Frame:**
```
┌──────────────────────────────────────────────────────────┐
│  CAN ID (11 bits)  │  DLC (4 bits)  │  Data (0-8 bytes) │
└──────────────────────────────────────────────────────────┘
     │                      │                  │
     │                      │                  └─ Message payload
     │                      └─ Data length code (0-8)
     └─ Message identifier (priority + type + source)
```

**ID Encoding:**
```
Bits 10-8: Priority (0=highest, 7=lowest)
Bits 7-4:  Message Type
Bits 3-0:  Source Node ID

Example:
  Priority 1, Type 0x5 (Telemetry), Node 0x2 (ADCS)
  ID = 0b00101010010 = 0x152
```

### Message Types

| ID Range | Type | Direction | Description |
|----------|------|-----------|-------------|
| 0x100-0x1FF | Commands | OBC → ADCS | Control commands |
| 0x200-0x2FF | Telemetry | ADCS → OBC | Status, sensor data |
| 0x300-0x3FF | Configuration | OBC ↔ ADCS | Parameter updates |
| 0x400-0x4FF | Diagnostics | ADCS → OBC | Debug, logs |

### Example Messages

**Command: Set Target Attitude**
```c
// CAN ID: 0x101 (Priority 0, Type 0x0, Command)
// DLC: 8 bytes
// Data: [q0_msb, q0_lsb, q1_msb, q1_lsb, q2_msb, q2_lsb, q3_msb, q3_lsb]

typedef struct {
    int16_t q0;  // Quaternion (scaled by 10000)
    int16_t q1;
    int16_t q2;
    int16_t q3;
} cmd_set_attitude_t;
```

**Telemetry: Attitude Status**
```c
// CAN ID: 0x201 (Priority 1, Type 0x0, Telemetry)
// DLC: 8 bytes
// Data: [q0_msb, q0_lsb, q1_msb, q1_lsb, mode, status, crc_msb, crc_lsb]

typedef struct {
    int16_t q0;        // Current attitude quaternion
    int16_t q1;
    uint8_t mode;      // Control mode (0=off, 1=detumb, 2=track)
    uint8_t status;    // Status flags
    uint16_t crc;      // CRC16
} telem_attitude_t;
```

---

## Control Loop Design

### Control Modes

```
┌─────────────┐
│   IDLE      │  Power-on state, coils disabled
└──────┬──────┘
       │ CMD: Start Detumbling
       ▼
┌─────────────┐
│  DETUMBLING │  B-dot controller, reduce angular velocity
└──────┬──────┘
       │ Angular velocity < threshold
       ▼
┌─────────────┐
│  TRACKING   │  PID controller, maintain target attitude
└──────┬──────┘
       │ CMD: Stop / Fault
       ▼
┌─────────────┐
│  SAFE MODE  │  Coils disabled, report fault
└─────────────┘
```

### Control Loop Timing

**10 ms Control Cycle:**
```
0ms    Read sensors from queues
1ms    Update Kalman filter (attitude estimation)
3ms    Run control algorithm (B-dot or PID)
4ms    Compute magnetorquer commands
5ms    Apply commands to actuators
6ms    Log telemetry
7ms    Check for faults
8ms    Yield to OS
10ms   Next cycle
```

---

## Memory Architecture

### Memory Map

```
STM32L476RG Memory Layout:

0x08000000  ┌────────────────────────┐
            │  Bootloader (16 KB)   │  (Optional)
0x08004000  ├────────────────────────┤
            │  Application Code      │
            │  (.text, .rodata)      │
            │  (~800 KB)             │
0x080FC000  ├────────────────────────┤
            │  Configuration         │
            │  (Non-volatile)        │
0x08100000  └────────────────────────┘  End of Flash (1 MB)

0x20000000  ┌────────────────────────┐
            │  .data (initialized)   │  ~4 KB
0x20001000  ├────────────────────────┤
            │  .bss (zeroed)         │  ~8 KB
0x20003000  ├────────────────────────┤
            │  Heap (malloc)         │  ~16 KB
0x20007000  ├────────────────────────┤
            │  FreeRTOS Stacks       │
            │  - Attitude Task       │  2 KB
            │  - Sensor Task         │  1 KB
            │  - CAN Task            │  1 KB
            │  - Health Task         │  1 KB
            │  - Watchdog Task       │  512 B
0x2000D000  ├────────────────────────┤
            │  FreeRTOS Kernel       │  ~8 KB
0x2000F000  ├────────────────────────┤
            │  Buffers & Queues      │  ~4 KB
0x20010000  ├────────────────────────┤
            │  Reserved              │
0x20018000  └────────────────────────┘  End of SRAM1 (96 KB)

0x10000000  ┌────────────────────────┐
            │  DMA Buffers           │
            │  (SRAM2, 32 KB)        │
0x10008000  └────────────────────────┘
```

### Data Structures

**Global Sensor Data:**
```c
typedef struct {
    mag_data_t magnetometer;
    gyro_data_t gyroscope;
    float sun_sensors[4];
    ina226_data_t current[4];
    float temperature[4];
    uint32_t timestamp;
} sensor_snapshot_t;
```

**Attitude State:**
```c
typedef struct {
    float quaternion[4];      // Orientation (q0, q1, q2, q3)
    float angular_vel[3];     // rad/s (ωx, ωy, ωz)
    float mag_field[3];       // µT (Bx, By, Bz)
    uint8_t mode;             // Control mode
    uint32_t timestamp;
} attitude_state_t;
```

---

## State Management

### System States

```c
typedef enum {
    SYS_STATE_INIT,          // Initialization
    SYS_STATE_IDLE,          // Idle, coils off
    SYS_STATE_DETUMBLING,    // Active detumbling
    SYS_STATE_TRACKING,      // Attitude tracking
    SYS_STATE_SAFE_MODE,     // Fault, coils disabled
    SYS_STATE_SHUTDOWN       // Controlled shutdown
} system_state_t;
```

### State Transitions

```
INIT → IDLE → DETUMBLING → TRACKING
  ↓      ↓         ↓           ↓
  └──────┴─────────┴───────────→ SAFE_MODE
                                      ↓
                                  SHUTDOWN
```

---

## Error Handling

### Fault Detection

**Hardware Faults:**
- Overcurrent detection (via INA226)
- Overtemperature detection (via TMP100)
- Sensor communication timeout (I2C/SPI)
- CAN bus off condition

**Software Faults:**
- Task watchdog timeout
- Stack overflow detection (FreeRTOS)
- Memory allocation failure
- Invalid data (CRC mismatch)

### Fault Response

```c
void handle_fault(fault_type_t fault) {
    switch (fault) {
        case FAULT_OVERCURRENT:
            // Disable affected coil immediately
            magnetorquer_disable_all();
            log_error("Overcurrent detected");
            break;

        case FAULT_OVERTEMP:
            // Reduce duty cycle or disable
            reduce_power();
            log_warning("Overtemperature");
            break;

        case FAULT_SENSOR_TIMEOUT:
            // Use last known good data
            use_backup_data();
            log_warning("Sensor timeout");
            break;

        case FAULT_CAN_BUS_OFF:
            // Attempt recovery
            can_recover();
            break;
    }

    // Enter safe mode if critical
    if (is_critical_fault(fault)) {
        enter_safe_mode();
    }
}
```

---

## Performance Metrics

### Timing Requirements

| Task | Period | WCET | Deadline | CPU Load |
|------|--------|------|----------|----------|
| Attitude Control | 10 ms | 5 ms | 10 ms | 50% |
| Sensor Read | 100 ms | 30 ms | 100 ms | 30% |
| CAN Communication | 50 ms | 10 ms | 50 ms | 20% |
| Health Monitor | 500 ms | 20 ms | 500 ms | 4% |
| Watchdog | 100 ms | 1 ms | 100 ms | 1% |
| **Total** | | | | **~105%** |

**Note:** Total >100% is acceptable due to non-overlapping execution with proper priority scheduling.

### Memory Usage

| Component | Flash (KB) | RAM (KB) |
|-----------|------------|----------|
| Application Code | ~200 | ~20 |
| STM32 HAL | ~100 | ~4 |
| FreeRTOS Kernel | ~10 | ~8 |
| Task Stacks | 0 | ~6 |
| Buffers & Queues | 0 | ~4 |
| **Total** | **~310** | **~42** |
| **Available** | 1024 | 128 |
| **Margin** | **~70%** | **~67%** |

---

## References

1. STM32L476RG Reference Manual (RM0351)
2. FreeRTOS Documentation
3. CAN 2.0B Specification
4. Spacecraft Attitude Determination and Control (Wertz)
5. Magnetorquer Control Algorithms (Lovera & Astolfi)

---

**Document maintained by:** ADCS Firmware Team
**For architecture changes, update this document and increment version.**
