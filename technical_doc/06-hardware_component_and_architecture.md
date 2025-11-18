# Hardware Components and Architecture

## Overview
This document describes the hardware architecture of the ADCS (Attitude Determination and Control System), including all major components, their specifications, interconnections, and design rationale.

## System Block Diagram

```
┌────────────────────────────────────────────────────────────────┐
│                        ADCS Board                              │
│                                                                │
│  ┌──────────────┐         ┌─────────────────────────┐        │
│  │   Power      │         │   STM32F303RCT6 MCU     │        │
│  │   Supply     │────────▶│   (ARM Cortex-M4F)      │        │
│  │   Regulator  │         │   - 72 MHz              │        │
│  │   (5V→3.3V)  │         │   - 256KB Flash         │        │
│  └──────────────┘         │   - 40KB RAM            │        │
│         ▲                 └──────────┬──────────────┘        │
│         │                            │                        │
│    ┌────┴─────┐                      │                        │
│    │  PC-104  │                      │                        │
│    │   Bus    │◀─────────────────────┤                        │
│    │  (+5V,   │      CAN Bus         │                        │
│    │  +12V)   │                      │                        │
│    └──────────┘                      │                        │
│                        ┌─────────────┴─────────────┐          │
│                        │                           │          │
│              ┌─────────▼────────┐      ┌──────────▼──────┐   │
│              │   I2C Bus        │      │   SPI Bus       │   │
│              │   (400 kHz)      │      │   (10 MHz)      │   │
│              └────┬─────────────┘      └────┬────────────┘   │
│                   │                          │                │
│         ┌─────────┼──────────┐              │                │
│         │         │          │       ┌──────┴──────┐         │
│    ┌────▼───┐ ┌──▼────┐ ┌──▼────┐  │             │         │
│    │ INA226 │ │INA226 │ │TMP100 │  │  I3G4250DTR │         │
│    │Voltage │ │Current│ │ Temp  │  │  Gyroscope  │         │
│    │Monitor │ │Monitor│ │Sensor │  │  (3-axis)   │         │
│    │  (4x)  │ │  (4x) │ │ (4x)  │  └─────────────┘         │
│    └────────┘ └───────┘ └───────┘                           │
│                                      ┌─────────────┐         │
│                                      │  MMC5983MA  │         │
│                                      │ Magnetometer│         │
│                                      │  (3-axis)   │         │
│                                      └─────────────┘         │
│                                                               │
│              ┌───────────────────────────────┐               │
│              │   H-Bridge Drivers (3x)       │               │
│              │   DRV8834PWP                  │               │
│              │   - X axis                    │               │
│              │   - Y axis                    │               │
│              │   - Z axis                    │               │
│              └───────┬───────────────────────┘               │
│                      │ PWM + Direction                       │
└──────────────────────┼───────────────────────────────────────┘
                       │
                ┌──────▼────────┐
                │ Magnetorquers │
                │   (External)  │
                │  - X coil     │
                │  - Y coil     │
                │  - Z coil     │
                └───────────────┘
```

## Major Components

### 1. Microcontroller Unit (MCU)

#### STM32F303RCT6
- **Manufacturer**: STMicroelectronics
- **Part Number**: STM32F303RCT6
- **Package**: LQFP64 (10mm × 10mm)
- **Core**: ARM Cortex-M4F with FPU
- **Clock Speed**: 72 MHz (internal PLL from 8 MHz crystal)
- **Flash Memory**: 256 KB
- **SRAM**: 40 KB (can be extended to 48 KB with CCM)
- **Operating Voltage**: 2.0V to 3.6V (nominal 3.3V)
- **Temperature Range**: -40°C to +85°C (industrial grade)

**Key Features**:
- Hardware floating-point unit (FPU) for efficient control algorithm computation
- Multiple communication interfaces (I2C, SPI, CAN, USART)
- Advanced timers for precise PWM generation
- 12-bit ADC (5 MSPS) for analog measurements
- DMA for efficient data transfer
- Low-power modes for power management

**Justification**: The FPU is essential for real-time attitude determination and control algorithms. The STM32F303 provides excellent peripheral support and proven spaceflight heritage in CubeSat missions.

**Cost** (2 units, 2022): $17.72

### 2. Sensors

#### 2.1 Gyroscope - I3G4250DTR

- **Manufacturer**: STMicroelectronics (MEMSIC)
- **Part Number**: I3G4250DTR
- **Type**: 3-axis MEMS gyroscope
- **Package**: LGA-16 (4mm × 4mm × 1mm)
- **Interface**: SPI or I2C
- **Supply Voltage**: 2.4V to 3.6V (3.3V nominal)
- **Current Consumption**: 6.1 mA (normal mode)

**Specifications**:
- **Measurement Range**: ±245/500/2000 dps (selectable)
- **Sensitivity**: 8.75 mdps/LSB @ ±245 dps
- **Zero-Rate Level**: ±10 dps
- **Output Data Rate**: Up to 800 Hz
- **Resolution**: 16-bit
- **Noise Density**: 0.03 dps/√Hz

**Justification**: Low-noise MEMS gyroscope with high data rate for accurate angular rate measurement. Essential for detumbling and attitude estimation.

**Cost** (2 units, 2022): $21.96

#### 2.2 Magnetometer - MMC5983MA

- **Manufacturer**: MEMSIC
- **Part Number**: MMC5983MA
- **Type**: 3-axis magnetoresistive magnetometer
- **Package**: WLCSP-12 (1.6mm × 1.6mm)
- **Interface**: SPI or I2C
- **Supply Voltage**: 2.6V to 3.6V (3.3V nominal)
- **Current Consumption**: 4 mA (active), 1 µA (standby)

**Specifications**:
- **Measurement Range**: ±8 Gauss (±800 µT)
- **Resolution**: 18-bit (0.0625 mG per LSB)
- **RMS Noise**: 0.4 mG @ 100 Hz
- **Magnetic Sensitivity**: 0.25 mV/V/Gauss
- **Output Data Rate**: Up to 1000 Hz
- **Temperature Coefficient**: 0.04% per °C

**Justification**: High-precision magnetometer with extremely low noise and wide dynamic range. Critical for Earth's magnetic field measurement and pointing control.

**Cost** (2 units, 2022): $8.88

#### 2.3 Voltage/Current Sensors - INA226 (8× units)

- **Manufacturer**: Texas Instruments
- **Part Number**: INA226AIDGSR
- **Type**: High-side current/voltage/power monitor
- **Package**: MSOP-10
- **Interface**: I2C
- **Supply Voltage**: 2.7V to 5.5V

**Specifications**:
- **Bus Voltage Range**: 0 to 36V
- **Shunt Voltage Range**: ±81.92 mV
- **Resolution**: 16-bit (both voltage and current)
- **Accuracy**: ±0.1% (voltage), ±0.1% (current)
- **I2C Address**: Configurable (4 address pins)
- **Alert Function**: Programmable thresholds

**Usage in ADCS**:
- **Quantity**: 8 units
  - 3× Magnetorquer current sensing (X, Y, Z)
  - 1× Bus voltage monitoring
  - 4× Additional voltage/current monitoring

**Justification**: Integrated current and voltage monitoring with high accuracy. I2C interface reduces MCU pin count. Alert function enables fast fault detection.

**Cost** (8 units × 2, 2022): $73.92

#### 2.4 Temperature Sensors - TMP100 (8× units)

- **Manufacturer**: Texas Instruments
- **Part Number**: TMP100NA/3K
- **Type**: Digital temperature sensor
- **Package**: SOT-23-6
- **Interface**: I2C
- **Supply Voltage**: 2.7V to 5.5V
- **Current Consumption**: 50 µA (active), 0.5 µA (standby)

**Specifications**:
- **Temperature Range**: -55°C to +125°C
- **Accuracy**: ±1°C (-55°C to +125°C)
- **Resolution**: 12-bit (0.0625°C)
- **Conversion Time**: 320 ms (12-bit)
- **I2C Address**: Configurable

**Usage in ADCS**:
- **Quantity**: 8 units
  - 1× MCU temperature
  - 3× Magnetorquer temperatures (X, Y, Z)
  - 4× Board zone temperatures

**Justification**: Low-cost, accurate digital temperature sensors. Multiple sensors enable thermal mapping and over-temperature protection.

**Cost** (8 units × 2, 2022): $21.44

### 3. Actuator Drivers

#### 3.1 H-Bridge Drivers - DRV8834PWP (6× units)

- **Manufacturer**: Texas Instruments
- **Part Number**: DRV8834PWP
- **Type**: Stepper motor / DC motor driver
- **Package**: HTSSOP-28
- **Supply Voltage**: 2.5V to 10.8V (VM), 3.3V logic
- **Current Rating**: 1.5A per phase (2A peak)

**Specifications**:
- **PWM Frequency**: Up to 250 kHz
- **Microstepping**: Up to 1/32 step (not used in ADCS)
- **Logic Interface**: 3.3V compatible
- **Protection**:
  - Over-current protection (OCP)
  - Thermal shutdown (TSD)
  - Under-voltage lockout (UVLO)
  - Short circuit protection
- **Fault Output**: Active-low fault indicator

**Configuration for ADCS**:
- **Quantity**: 6 units (2 per magnetorquer axis)
- **Mode**: Full H-bridge for bidirectional current control
- **PWM Input**: From STM32 TIM1 channels
- **Direction Control**: 2 GPIO per bridge (IN1, IN2)
- **Current Sensing**: Via INA226 on power path

**Justification**: Integrated H-bridge with built-in protection features. High efficiency (>90%) reduces power dissipation. Simple PWM + direction interface.

**Cost** (6 units × 2, 2022): $28.96

### 4. Communication Interfaces

#### 4.1 CAN Transceiver - TCAN330GD (2× units)

- **Manufacturer**: Texas Instruments
- **Part Number**: TCAN330GD
- **Type**: CAN transceiver
- **Package**: SOIC-8
- **Supply Voltage**: 3.0V to 3.6V (VCC), -2V to 7V (VIO)
- **Data Rate**: Up to 1 Mbps

**Specifications**:
- **Bus Voltage**: ±12V (differential)
- **Common-Mode Range**: -2V to 7V
- **ESD Protection**: ±8 kV HBM
- **Low-Power Mode**: <10 µA standby current
- **Thermal Shutdown**: >165°C
- **Fault Protection**: Short circuit, over-voltage

**Justification**: Robust CAN transceiver with industrial-grade ESD protection. Essential for reliable communication with OBC in electrically noisy CubeSat environment.

**Cost** (2 units × 2, 2022): $7.78

### 5. Power Management

#### 5.1 Voltage Regulator

- **Input**: +5V or +12V from PC-104 bus
- **Output**: +3.3V (regulated)
- **Current Capacity**: ~1A (sufficient for all loads)
- **Type**: Linear LDO or switching regulator (TBD based on efficiency requirements)

**Load Estimation**:
- MCU: 100 mA
- Sensors (gyro, mag, INA226, TMP100): 50 mA
- CAN transceiver: 10 mA
- H-bridge logic: 20 mA
- **Total 3.3V Rail**: ~200 mA (with margin: 500 mA regulator)

**Magnetorquer Power**:
- Supplied directly from +12V bus
- Peak current: ~1.5A per axis (worst case: all 3 axes = 4.5A)
- Average current: ~500 mA (typical)

#### 5.2 External Watchdog - TPS3823

- **Manufacturer**: Texas Instruments
- **Part Number**: TPS3823-30DBVR
- **Type**: Voltage supervisor with watchdog timer
- **Package**: SOT-23-5
- **Supply Voltage**: 1.0V to 6.0V

**Specifications**:
- **Watchdog Timeout**: 1600 ms (typical)
- **Reset Pulse Width**: 400 ms
- **Voltage Threshold**: 3.0V (for 3.3V monitoring)
- **Quiescent Current**: 12 µA
- **Watchdog Input**: Active-high or toggle

**Justification**: Independent hardware watchdog ensures MCU reliability. Automatic reset on firmware hang or crash. Critical for spacecraft autonomy.

**Cost** (2 units, 2022): $3.18

### 6. Passive Components

#### 6.1 Resistors
- **Quantity**: ~50
- **Types**:
  - Pull-up/pull-down resistors (4.7kΩ, 10kΩ)
  - Current sense shunt resistors (0.1Ω, 1W)
  - LED current limiters (470Ω)
  - I2C pull-ups (4.7kΩ)
- **Package**: 0603 or 0805 (SMD)

#### 6.2 Capacitors
- **Quantity**: ~40
- **Types**:
  - Decoupling capacitors: 100nF (ceramic X7R)
  - Bulk capacitors: 10µF, 47µF (tantalum or ceramic)
  - Crystal load capacitors: 22pF (C0G/NP0)
  - Power supply filtering: 100µF electrolytic
- **Package**: 0603, 0805, 1206 (SMD)

#### 6.3 Crystals and Oscillators
- **Main Crystal**: 8 MHz (for MCU PLL)
  - Package: HC-49S SMD
  - Accuracy: ±20 ppm
  - Load capacitance: 20 pF
- **RTC Crystal** (optional): 32.768 kHz
  - Package: Cylindrical SMD
  - Accuracy: ±20 ppm

#### 6.4 Inductors
- **Quantity**: ~5
- **Types**:
  - Power supply filtering: 10µH, 1A
  - VDDA ferrite bead: 600Ω @ 100 MHz
- **Package**: 1210, 1812 (SMD)

**Total Passive Components Cost** (estimate, 2 units): ~$10

### 7. Magnetorquer Coils (External, Off-Board)

#### 7.1 X-Axis Magnetorquer
- **Type**: Soft magnetic core (ferrite or permalloy)
- **Core Material**: High-permeability ferrite (µr ≈ 2000-5000)
- **Winding**: Copper wire, AWG 28-32
- **Inductance**: ~100 mH (typical)
- **Resistance**: ~10Ω (DC)
- **Magnetic Dipole Moment**: ≥0.2 A·m² @ 1A
- **Dimensions**: ~80mm × 10mm × 10mm (TBD)

#### 7.2 Y-Axis Magnetorquer
- Similar to X-axis

#### 7.3 Z-Axis Magnetorquer
- **Type**: Air-core solenoid
- **Winding**: Copper wire, AWG 28-32
- **Inductance**: ~50 mH (typical)
- **Resistance**: ~8Ω (DC)
- **Magnetic Dipole Moment**: ≥0.2 A·m² @ 1A
- **Dimensions**: ~85mm diameter, 10mm height (TBD)

**Justification**: X and Y axes use magnetic cores for higher efficiency. Z-axis uses air core to avoid interference with magnetometer (perpendicular mounting). Detailed magnetorquer design is documented in `/doc/magnetorquer-sizing/`.

### 8. Connectors

#### 8.1 PC-104 Connector
- **Type**: PC-104 stackthrough connector (2mm pitch)
- **Pins**: 64 pins (power, signals, CAN bus)
- **Manufacturer**: Samtec or equivalent
- **Mechanical**: Supports stacking with other PC-104 modules

#### 8.2 Magnetorquer Connectors
- **Type**: Wire-to-board screw terminal or locking connector
- **Pins**: 2 per coil (6 total)
- **Current Rating**: ≥2A per contact
- **Voltage Rating**: ≥20V

#### 8.3 Debug Header
- **Type**: 2.54mm (0.1") pin header
- **Pins**: 6-pin (SWD + UART)
- **Layout**: 2×3 or 1×6

## Hardware Architecture Details

### Power Distribution Architecture

```
PC-104 Bus
  │
  ├──[+5V]───▶ 3.3V Regulator ───▶ +3.3V Rail
  │                                    │
  │                                    ├─▶ MCU (STM32)
  │                                    ├─▶ Sensors (Gyro, Mag, INA226, TMP100)
  │                                    ├─▶ CAN Transceiver
  │                                    └─▶ H-Bridge Logic
  │
  └──[+12V]───▶ Protection ──────▶ H-Bridge Drivers ──▶ Magnetorquers
               (Fuse, TVS)
```

### Sensor Interface Architecture

```
MCU (STM32F303)
  │
  ├──[SPI1]───┬─▶ Gyroscope (I3G4250DTR)
  │           └─▶ Magnetometer (MMC5983MA)
  │
  ├──[I2C1]───┬─▶ INA226 (Address 0x40) - Coil X Current
  │           ├─▶ INA226 (Address 0x41) - Coil Y Current
  │           ├─▶ INA226 (Address 0x42) - Coil Z Current
  │           ├─▶ INA226 (Address 0x43) - Bus Voltage
  │           ├─▶ TMP100 (Address 0x48) - MCU Temp
  │           ├─▶ TMP100 (Address 0x49) - Coil X Temp
  │           ├─▶ TMP100 (Address 0x4A) - Coil Y Temp
  │           └─▶ TMP100 (Address 0x4B) - Coil Z Temp
  │
  └──[CAN]────▶ TCAN330GD ──▶ PC-104 CAN Bus
```

### Actuator Control Architecture

```
MCU (STM32F303)
  │
  ├──[TIM1_CH1 PWM]──┐
  ├──[GPIO: IN1/IN2]─┼─▶ DRV8834 (X-axis) ──▶ Magnetorquer X
  ├──[GPIO: FAULT]◀──┘
  │
  ├──[TIM1_CH2 PWM]──┐
  ├──[GPIO: IN1/IN2]─┼─▶ DRV8834 (Y-axis) ──▶ Magnetorquer Y
  ├──[GPIO: FAULT]◀──┘
  │
  ├──[TIM1_CH3 PWM]──┐
  ├──[GPIO: IN1/IN2]─┼─▶ DRV8834 (Z-axis) ──▶ Magnetorquer Z
  └──[GPIO: FAULT]◀──┘
```

## PCB Design Specifications

### Board Specifications
- **Form Factor**: PC-104 (90mm × 90mm)
- **Layers**: 4-layer PCB
  - Layer 1: Top signal layer
  - Layer 2: Ground plane
  - Layer 3: Power plane (+3.3V, +12V)
  - Layer 4: Bottom signal layer
- **Thickness**: 1.6mm standard
- **Copper Weight**: 1 oz (35 µm) for signal layers, 2 oz (70 µm) for power planes (optional)
- **Minimum Trace Width**: 6 mil (0.15mm) for signals, 20 mil (0.5mm) for power
- **Minimum Via Size**: 12 mil drill, 24 mil pad
- **Solder Mask**: Green (or space-grade black)
- **Silkscreen**: White on top and bottom

### Design Files Location
- **Schematic**: `/home/user/adcs/hardware/sources/adcs.SchDoc`
- **PCB Layout**: `/home/user/adcs/hardware/sources/adcs.PcbDoc`
- **Libraries**: `/home/user/adcs/hardware/libraries/`
- **Manufacturing Outputs**: Generated via `adcs_outjob.OutJob`

### Component Placement Strategy
1. **MCU**: Central location for shortest trace lengths to peripherals
2. **Sensors**: Away from magnetorquers and high-current traces
3. **H-Bridges**: Near board edge for heat dissipation
4. **Power Regulator**: Near PC-104 connector input
5. **Decoupling Capacitors**: As close as possible to IC power pins

### Thermal Considerations
- **H-Bridge Thermal Pads**: Connected to bottom copper pour (ground or thermal relief)
- **MCU Thermal Management**: Thermal via array under MCU for heat spreading
- **Temperature Monitoring**: TMP100 sensors placed near heat sources

## Bill of Materials (BOM) Summary

| Category | Quantity | Unit Cost | Total Cost (2 units) |
|----------|----------|-----------|----------------------|
| MCU (STM32F303RCT6) | 2 | $8.86 | $17.72 |
| Gyroscope (I3G4250DTR) | 2 | $10.98 | $21.96 |
| Magnetometer (MMC5983MA) | 2 | $4.44 | $8.88 |
| INA226 (8× units) | 16 | $4.62 | $73.92 |
| TMP100 (8× units) | 16 | $1.34 | $21.44 |
| DRV8834PWP (6× units) | 12 | $2.41 | $28.96 |
| TCAN330GD (2× units) | 4 | $1.95 | $7.78 |
| TPS3823 Watchdog | 2 | $1.59 | $3.18 |
| Passives & Misc | - | - | ~$10.00 |
| **Total** | - | - | **~$193.84** |

**Note**: Prices from August 2022. Actual costs may vary.

## Manufacturing and Assembly

### PCB Fabrication
- **Vendor**: Standard PCB fab house (e.g., JLCPCB, PCBWay, or aerospace-certified vendor)
- **Lead Time**: 1-2 weeks (standard), 3-5 days (expedited)
- **Quantity**: 2-5 boards recommended (for testing and spares)

### Assembly
- **Method**: SMT pick-and-place + manual soldering
- **Reflow Profile**: Lead-free SAC305 or leaded Sn63/Pb37
- **Inspection**: AOI (Automated Optical Inspection) + manual visual inspection
- **Testing**: Electrical test, functional test, burn-in

### Testing and Qualification
1. **Continuity Test**: Verify all connections
2. **Power-On Test**: Check voltage rails
3. **Sensor Communication**: Verify I2C and SPI devices
4. **Actuator Test**: PWM generation and current control
5. **CAN Communication**: Loopback and OBC interface test
6. **Thermal Test**: Verify temperature sensors and thermal limits
7. **Vibration Test**: Simulate launch loads (optional for flight units)

## Design Trade-offs and Decisions

### 1. MCU Selection: STM32F303 vs Alternatives
**Decision**: STM32F303RCT6
**Rationale**:
- Hardware FPU critical for control algorithms
- Proven spaceflight heritage
- Excellent peripheral support (CAN, SPI, I2C)
- Affordable and readily available

**Alternatives Considered**:
- STM32L476: Lower power but less compute performance
- STM32F4: More powerful but higher cost and power

### 2. Sensor Interface: SPI vs I2C
**Decision**: SPI for gyro and magnetometer, I2C for housekeeping
**Rationale**:
- SPI provides higher data rate (100 Hz sensor update)
- I2C simplifies wiring for multiple housekeeping sensors (unique addresses)
- I2C adequate for 1 Hz housekeeping data

### 3. H-Bridge Driver: Integrated vs Discrete
**Decision**: Integrated DRV8834PWP
**Rationale**:
- Built-in protection (OCP, TSD, UVLO)
- Smaller footprint than discrete MOSFETs
- Simplified design and lower part count
- Proven reliability

**Alternative**: Discrete H-bridge with MOSFETs (more complex, higher power)

### 4. Magnetorquer Design: Cored vs Air-Core
**Decision**: Hybrid (X/Y cored, Z air-core)
**Rationale**:
- Cored coils: Higher efficiency (µr gain)
- Air-core Z: Avoids magnetometer interference
- Detailed analysis in `/doc/magnetorquer-sizing/`

### 5. Power Supply: Linear vs Switching
**Decision**: TBD (Linear LDO for simplicity, switching for efficiency)
**Rationale**:
- Linear: Simple, low noise, adequate for <1W load
- Switching: Higher efficiency (important for power budget)
- Trade-off: Noise vs efficiency

## Compliance and Standards

### CubeSat Standards
- **PC-104**: Compliant with PC-104 mechanical and electrical spec
- **Form Factor**: 90mm × 90mm, 3U CubeSat compatible

### Environmental
- **Operating Temperature**: -20°C to +60°C (typical spacecraft internal)
- **Storage Temperature**: -40°C to +85°C
- **Humidity**: Non-condensing (N/A in space)

### EMI/EMC
- **Conducted Emissions**: Minimize via proper grounding and filtering
- **Radiated Emissions**: Keep PWM frequency >20 kHz (ultrasonic)
- **ESD Protection**: TVS diodes on external interfaces

### Safety
- **Over-Current Protection**: Fuses and current monitoring
- **Over-Temperature Protection**: Temperature sensors and shutdown
- **Fault Isolation**: Watchdog and software fault handling

## Future Enhancements

### Possible Upgrades
1. **Sun Sensors**: Add photodiode array for sun vector determination
2. **Star Tracker Interface**: UART or SPI for future star tracker integration
3. **Reaction Wheel Interface**: PWM or UART for hybrid ADCS
4. **SD Card Logging**: SPI SD card for data logging
5. **GPS Interface**: UART for absolute time and position (LEO orbits)

### Redundancy Options
1. **Dual MCU**: Cold spare MCU with failover logic
2. **Dual CAN**: Redundant CAN bus for mission-critical communication
3. **Backup Power**: Supercapacitor for ride-through during power transients

## Reference Documents

- **Schematics**: `/home/user/adcs/hardware/sources/` (Altium Designer files)
- **PDR Document**: `/home/user/adcs/doc/pdr/main.pdf`
- **Magnetorquer Design**: `/home/user/adcs/doc/magnetorquer-sizing/magnetorquer-sizing.ipynb`
- **Component Datasheets**: Manufacturer websites
  - STM32F303: [STMicroelectronics](https://www.st.com)
  - DRV8834: [Texas Instruments](https://www.ti.com)
  - INA226, TMP100: [Texas Instruments](https://www.ti.com)
