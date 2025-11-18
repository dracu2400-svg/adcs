# RTOS Tasks and Scheduling

## Overview
This document describes the task structure for the ADCS firmware, including task priorities, execution rates, and inter-task communication mechanisms. The firmware is designed to run on an RTOS (e.g., FreeRTOS) for deterministic real-time performance.

## Task Architecture

```
┌────────────────────────────────────────────────────────┐
│                   RTOS Scheduler                       │
│          (Preemptive Priority-based Scheduling)        │
└────────────────────────────────────────────────────────┘
     │          │          │          │          │
     ▼          ▼          ▼          ▼          ▼
┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐
│ Watchdog│ │ Sensor  │ │ Control │ │  CAN    │ │ Telemetry│
│  Task   │ │  Task   │ │  Task   │ │  Task   │ │   Task   │
│ (High)  │ │ (High)  │ │ (Medium)│ │ (Medium)│ │  (Low)   │
│ 10 Hz   │ │ 100 Hz  │ │  10 Hz  │ │  Event  │ │   1 Hz   │
└─────────┘ └─────────┘ └─────────┘ └─────────┘ └─────────┘
```

## Task Definitions

### 1. Watchdog Task

**Priority**: Highest (configMAX_PRIORITIES - 1)
**Stack Size**: 256 bytes
**Period**: 100 ms (10 Hz)
**Execution Time**: <1 ms

#### Responsibilities
- Monitor system health
- Kick external watchdog (TPS3823)
- Verify all critical tasks are running
- Detect task starvation or deadlocks
- Increment system uptime counter

#### Watchdog Kick Logic
```c
// Pseudocode
void WatchdogTask(void *params) {
    TickType_t lastWakeTime = xTaskGetTickCount();

    while(1) {
        // Check if all tasks are healthy
        if (CheckTaskHealth()) {
            // Toggle watchdog GPIO
            HAL_GPIO_TogglePin(WDI_GPIO_Port, WDI_Pin);
        } else {
            // Don't kick watchdog - allow timeout and reset
            LogFault(FAULT_TASK_STUCK);
        }

        // Update system uptime
        systemUptime_ms += 100;

        // Sleep until next period
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(100));
    }
}
```

#### Task Health Monitoring
Each task sets a heartbeat flag when it executes. Watchdog task verifies all flags are toggled.

```c
typedef struct {
    uint32_t sensor_heartbeat;
    uint32_t control_heartbeat;
    uint32_t can_heartbeat;
    uint32_t telemetry_heartbeat;
} TaskHealthStatus_t;
```

### 2. Sensor Reading Task

**Priority**: High (configMAX_PRIORITIES - 2)
**Stack Size**: 1024 bytes
**Period**: 10 ms (100 Hz)
**Execution Time**: ~2-3 ms

#### Responsibilities
- Read gyroscope data (I3G4250DTR via SPI)
- Read magnetometer data (MMC5983MA via SPI/I2C)
- Read sun sensor data (TBD)
- Apply calibration and filtering
- Publish data to shared memory / queue
- Update sensor heartbeat flag

#### Execution Flow
```c
void SensorTask(void *params) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    SensorData_t sensorData;

    while(1) {
        // Read gyroscope (SPI transaction)
        Gyro_ReadData(&sensorData.gyro);

        // Read magnetometer (SPI/I2C transaction)
        Mag_ReadData(&sensorData.mag);

        // Read sun sensor (TBD)
        // SunSensor_ReadData(&sensorData.sun);

        // Apply calibration
        ApplyGyroCalibration(&sensorData.gyro);
        ApplyMagCalibration(&sensorData.mag);

        // Apply filtering
        FilterSensorData(&sensorData);

        // Publish to control task (thread-safe)
        xSemaphoreTake(sensorDataMutex, portMAX_DELAY);
        memcpy(&sharedSensorData, &sensorData, sizeof(SensorData_t));
        sensorDataReady = true;
        xSemaphoreGive(sensorDataMutex);

        // Update heartbeat
        taskHealthStatus.sensor_heartbeat++;

        // Sleep until next period (10 ms)
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(10));
    }
}
```

#### Sensor Data Structure
```c
typedef struct {
    struct {
        float omega_x; // rad/s
        float omega_y;
        float omega_z;
        uint32_t timestamp;
        bool valid;
    } gyro;

    struct {
        float B_x; // Gauss
        float B_y;
        float B_z;
        uint32_t timestamp;
        bool valid;
    } mag;

    struct {
        float sun_x; // Unit vector
        float sun_y;
        float sun_z;
        uint32_t timestamp;
        bool valid;
    } sun;
} SensorData_t;
```

### 3. Control Task

**Priority**: Medium-High (configMAX_PRIORITIES - 3)
**Stack Size**: 2048 bytes (needs stack for floating-point calculations)
**Period**: 100 ms (10 Hz)
**Execution Time**: ~10-20 ms (depends on control mode)

#### Responsibilities
- Attitude estimation (sensor fusion)
- Execute control algorithm (detumbling or pointing)
- Generate magnetorquer commands
- Send commands to actuator driver
- Update control heartbeat flag

#### Execution Flow
```c
void ControlTask(void *params) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    SensorData_t sensorData;
    AttitudeState_t attitudeState;
    ControlOutput_t controlOutput;

    while(1) {
        // Get latest sensor data (thread-safe)
        xSemaphoreTake(sensorDataMutex, portMAX_DELAY);
        memcpy(&sensorData, &sharedSensorData, sizeof(SensorData_t));
        xSemaphoreGive(sensorDataMutex);

        // Attitude estimation
        EstimateAttitude(&sensorData, &attitudeState);

        // Execute control based on mode
        switch (systemMode) {
            case MODE_IDLE:
                // No control, zero output
                memset(&controlOutput, 0, sizeof(ControlOutput_t));
                break;

            case MODE_DETUMBLING:
                // B-dot control
                DetumblingControl(&sensorData, &controlOutput);
                break;

            case MODE_POINTING:
                // PID pointing control
                PointingControl(&attitudeState, &targetAttitude, &controlOutput);
                break;

            default:
                // Safe mode - zero output
                memset(&controlOutput, 0, sizeof(ControlOutput_t));
                break;
        }

        // Send commands to magnetorquer driver
        Magnetorquer_SetCommands(&controlOutput);

        // Store for telemetry
        xSemaphoreTake(controlDataMutex, portMAX_DELAY);
        memcpy(&sharedAttitudeState, &attitudeState, sizeof(AttitudeState_t));
        memcpy(&sharedControlOutput, &controlOutput, sizeof(ControlOutput_t));
        xSemaphoreGive(controlDataMutex);

        // Update heartbeat
        taskHealthStatus.control_heartbeat++;

        // Sleep until next period (100 ms)
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(100));
    }
}
```

#### Control Data Structures
```c
typedef struct {
    float q0, q1, q2, q3; // Attitude quaternion
    float omega_x, omega_y, omega_z; // Angular rates (rad/s)
    uint32_t timestamp;
    bool valid;
} AttitudeState_t;

typedef struct {
    float M_x; // Magnetic dipole moment (A·m²)
    float M_y;
    float M_z;
    uint32_t timestamp;
} ControlOutput_t;
```

### 4. CAN Communication Task

**Priority**: Medium (configMAX_PRIORITIES - 4)
**Stack Size**: 1024 bytes
**Period**: Event-driven (triggered by CAN RX interrupt) + 1 Hz periodic heartbeat
**Execution Time**: <5 ms per message

#### Responsibilities
- Process incoming CAN messages (commands from OBC)
- Send periodic telemetry heartbeat (1 Hz)
- Handle command parsing and validation
- Trigger mode changes
- Update CAN heartbeat flag

#### Execution Flow
```c
void CANTask(void *params) {
    TickType_t lastHeartbeat = xTaskGetTickCount();
    CANMessage_t rxMsg;

    while(1) {
        // Wait for CAN RX event or timeout (1 second)
        if (xQueueReceive(canRxQueue, &rxMsg, pdMS_TO_TICKS(1000)) == pdTRUE) {
            // Process received message
            ProcessCANCommand(&rxMsg);
        }

        // Send periodic heartbeat (1 Hz)
        if ((xTaskGetTickCount() - lastHeartbeat) >= pdMS_TO_TICKS(1000)) {
            SendCANHeartbeat();
            lastHeartbeat = xTaskGetTickCount();
        }

        // Update heartbeat
        taskHealthStatus.can_heartbeat++;
    }
}
```

#### Command Processing
```c
void ProcessCANCommand(CANMessage_t *msg) {
    switch (msg->id) {
        case CAN_ID_MODE_CHANGE:
            if (msg->dlc == 1) {
                ChangeSystemMode(msg->data[0]);
            }
            break;

        case CAN_ID_SET_TARGET:
            if (msg->dlc == 8) {
                UpdateTargetAttitude(msg->data);
            }
            break;

        case CAN_ID_REQUEST_TLM:
            if (msg->dlc == 1) {
                SendRequestedTelemetry(msg->data[0]);
            }
            break;

        case CAN_ID_CALIBRATE:
            if (msg->dlc == 2) {
                TriggerCalibration(msg->data[0], msg->data[1]);
            }
            break;

        default:
            // Unknown command
            LogError(ERROR_UNKNOWN_CAN_CMD, msg->id);
            break;
    }
}
```

#### CAN RX Interrupt Handler (ISR)
```c
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CANMessage_t rxMsg;

    // Read message from CAN peripheral
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxMsg.header, rxMsg.data);

    // Post to queue for task processing
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(canRxQueue, &rxMsg, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}
```

### 5. Telemetry Task

**Priority**: Low (configMAX_PRIORITIES - 5)
**Stack Size**: 1024 bytes
**Period**: 1000 ms (1 Hz)
**Execution Time**: ~5-10 ms

#### Responsibilities
- Collect telemetry from all subsystems
- Read housekeeping sensors (INA226, TMP100 via I2C)
- Format telemetry messages
- Send telemetry via CAN bus
- Update telemetry heartbeat flag

#### Execution Flow
```c
void TelemetryTask(void *params) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    TelemetryPacket_t tlmPacket;

    while(1) {
        // Read housekeeping sensors (I2C transactions)
        ReadVoltageSensors(&tlmPacket.voltages);
        ReadCurrentSensors(&tlmPacket.currents);
        ReadTemperatureSensors(&tlmPacket.temperatures);

        // Get attitude data from control task
        xSemaphoreTake(controlDataMutex, portMAX_DELAY);
        memcpy(&tlmPacket.attitude, &sharedAttitudeState, sizeof(AttitudeState_t));
        memcpy(&tlmPacket.controlOutput, &sharedControlOutput, sizeof(ControlOutput_t));
        xSemaphoreGive(controlDataMutex);

        // Get sensor data
        xSemaphoreTake(sensorDataMutex, portMAX_DELAY);
        memcpy(&tlmPacket.sensors, &sharedSensorData, sizeof(SensorData_t));
        xSemaphoreGive(sensorDataMutex);

        // Add system status
        tlmPacket.systemMode = systemMode;
        tlmPacket.uptime_s = systemUptime_ms / 1000;
        tlmPacket.errorFlags = systemErrorFlags;

        // Send telemetry packets via CAN
        SendTelemetryPackets(&tlmPacket);

        // Update heartbeat
        taskHealthStatus.telemetry_heartbeat++;

        // Sleep until next period (1000 ms)
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000));
    }
}
```

### 6. Housekeeping Task (Optional - can be merged with Telemetry)

**Priority**: Low (configMAX_PRIORITIES - 6)
**Stack Size**: 512 bytes
**Period**: 1000 ms (1 Hz)
**Execution Time**: ~5 ms

#### Responsibilities
- Monitor temperatures for over-temperature
- Monitor voltages for under/over-voltage
- Monitor currents for over-current
- Trigger fault handling if thresholds exceeded
- Log min/max values

## Task Priority Summary

| Task | Priority | Period | Stack | Execution Time | Deadline |
|------|----------|--------|-------|----------------|----------|
| Watchdog | 5 (Highest) | 100 ms | 256 B | <1 ms | 100 ms |
| Sensor | 4 (High) | 10 ms | 1024 B | 2-3 ms | 10 ms |
| Control | 3 (Medium-High) | 100 ms | 2048 B | 10-20 ms | 100 ms |
| CAN | 2 (Medium) | Event + 1s | 1024 B | <5 ms | N/A |
| Telemetry | 1 (Low) | 1000 ms | 1024 B | 5-10 ms | 1000 ms |

**Total Stack Usage**: ~5.5 KB (of 40 KB RAM available)

## Inter-Task Communication

### 1. Shared Memory with Mutex Protection

**Sensor Data**: Protected by `sensorDataMutex`
- Written by: Sensor Task
- Read by: Control Task, Telemetry Task

**Control Data**: Protected by `controlDataMutex`
- Written by: Control Task
- Read by: Telemetry Task

**System Mode**: Protected by `systemModeMutex`
- Written by: CAN Task (on command)
- Read by: Control Task, Telemetry Task

### 2. Queues

**CAN RX Queue**: `canRxQueue`
- Writer: CAN RX ISR
- Reader: CAN Task
- Depth: 10 messages
- Item Size: sizeof(CANMessage_t) = ~16 bytes

**Error Log Queue**: `errorLogQueue`
- Writers: All tasks
- Reader: Telemetry Task (or dedicated logging task)
- Depth: 20 errors
- Item Size: sizeof(ErrorLog_t) = ~8 bytes

### 3. Event Flags (Optional)

**Data Ready Flags**: Using FreeRTOS event groups
- `sensorDataReady`: Set by Sensor Task, cleared by Control Task
- `tlmDataReady`: Set by Telemetry Task, cleared by CAN Task

## Timing Analysis

### Worst-Case Schedulability

**Assumptions**:
- Preemptive priority-based scheduling
- Higher priority tasks can preempt lower priority tasks
- No priority inversion (mutexes use priority inheritance)

**Timeline Example (10 ms window)**:
```
Time (ms)  0    1    2    3    4    5    6    7    8    9    10
           |----|----|----|----|----|----|----|----|----|----|
Sensor     [===]                                          [===]
Watchdog                             [=]
Control
Telemetry
CAN                       [==]
```

**CPU Utilization** (Rate Monotonic Analysis):
- Sensor: 3 ms / 10 ms = 30%
- Watchdog: 1 ms / 100 ms = 1%
- Control: 20 ms / 100 ms = 20%
- Telemetry: 10 ms / 1000 ms = 1%
- CAN: 5 ms / (event-driven) ≈ 1% average
- **Total: ~53%** (safe margin for 72 MHz Cortex-M4)

### Critical Paths

1. **Sensor → Control Path**:
   - Sensor read: 3 ms
   - Mutex lock/copy: <0.1 ms
   - Control computation: 20 ms
   - **Total Latency: ~23 ms** (acceptable for 10 Hz control)

2. **Command → Response Path**:
   - CAN RX ISR: <0.1 ms
   - Queue post: <0.1 ms
   - CAN task wakes: <1 ms (context switch)
   - Command processing: <2 ms
   - **Total Latency: ~3 ms** (acceptable for commands)

## RTOS Configuration (FreeRTOS Example)

```c
// FreeRTOSConfig.h excerpt
#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCPU_CLOCK_HZ                      72000000
#define configTICK_RATE_HZ                      1000  // 1 ms tick
#define configMAX_PRIORITIES                    6
#define configMINIMAL_STACK_SIZE                128
#define configTOTAL_HEAP_SIZE                   10240  // 10 KB
#define configUSE_16_BIT_TICKS                  0
#define configUSE_MUTEXES                       1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_QUEUE_SETS                    0
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_TRACE_FACILITY                1
```

## Fault Handling and Safe Mode

### Fault Detection
- Task starvation (detected by Watchdog Task)
- Sensor communication failure
- Over-temperature, over-current
- Invalid data from sensors

### Safe Mode Actions
1. **Disable all actuators** (magnetorquers off)
2. **Transition to IDLE mode**
3. **Log fault to error queue**
4. **Notify OBC via CAN**
5. **Attempt recovery** (e.g., sensor re-init)
6. **If recovery fails**: Allow watchdog timeout and reset

### Recovery Strategy
- Retry sensor initialization (up to 3 times)
- Clear error flags after successful recovery
- Resume normal operation
- Log recovery event for diagnostics

## Development and Testing

### Task Monitoring Tools
- FreeRTOS runtime stats (CPU usage per task)
- Stack high-water mark checking
- Task list enumeration
- Heap usage monitoring

### Debug Commands (via UART or CAN)
- List tasks and status
- Print stack usage
- Print CPU utilization
- Toggle task execution (for testing)
- Inject faults (for validation)

### Unit Testing
- Individual task functions tested in isolation
- Mock RTOS calls for host-based testing
- Hardware-in-loop (HIL) testing on target MCU
