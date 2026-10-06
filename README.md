# Virtual Sensor Simulator

## Purpose

This project implements a deterministic virtual temperature sensor simulator in C. The simulator generates temperature readings based on a selected operating mode and classifies the readings according to predefined thresholds.

The purpose of the simulator is to demonstrate sensor simulation, status classification, input validation, and testing without requiring physical sensor hardware.

## Build

The program can be compiled using GCC:

```bash
gcc sensor_sim.c -o sensor_sim
```

## Usage

Run the simulator using:

```bash
./sensor_sim <mode> <sample_count>
```

The supported modes are:

* `normal`
* `warning`
* `failure`

The sample count must be between 1 and 100.

Example:

```bash
./sensor_sim normal 5
```

## Mode Behavior

### Normal Mode

Normal mode generates deterministic temperature readings within the expected normal operating range. These readings should be classified as `NORMAL`.

### Warning Mode

Warning mode generates readings that fall within the warning range. These readings are classified as `WARNING`.

### Failure Mode

Failure mode produces the defined sensor failure value. This allows the simulator to demonstrate how a sensor failure condition is detected and classified.

## Thresholds and Units

Temperature values are represented in milli-degrees Celsius (m°C).

The simulator uses the following thresholds:

| Constant          |         Value | Description             |
| ----------------- | ------------: | ----------------------- |
| `WARNING_LOW_mC`  |   `18000 m°C` | Lower warning threshold |
| `WARNING_HIGH_mC` |   `30000 m°C` | Upper warning threshold |
| `FAILURE_CODE_mC` | `-999000 m°C` | Sensor failure value    |

These thresholds are used to determine the status of each simulated temperature reading.

## Example Output

Example normal-mode execution:

```text
Mode: normal
Samples: 5

Sample 1: temperature = 22000 mC, status = NORMAL
Sample 2: temperature = 22500 mC, status = NORMAL
Sample 3: temperature = 23000 mC, status = NORMAL
Sample 4: temperature = 23500 mC, status = NORMAL
Sample 5: temperature = 24000 mC, status = NORMAL
```

The exact output depends on the deterministic readings implemented in `sensor_sim.c`.

## Design Decisions

The simulator uses deterministic readings instead of random values so that the same inputs produce predictable results. This makes the program easier to test and allows expected results to be compared consistently.

Separate modes are used to demonstrate normal, warning, and failure conditions. Input validation prevents unsupported modes and invalid sample counts from being processed.

Temperature values are represented in milli-degrees Celsius to avoid floating-point calculations while still allowing precise temperature values.

## Test Procedure

The simulator was tested using all supported modes.

### Normal Mode

```bash
./sensor_sim normal 5
```

Verify that the readings are classified as `NORMAL`.

### Warning Mode

```bash
./sensor_sim warning 5
```

Verify that the readings are classified as `WARNING`.

### Failure Mode

```bash
./sensor_sim failure 5
```

Verify that the failure value is detected and classified appropriately.

### Invalid Mode

Run the simulator using an unsupported mode and verify that the program rejects the input.

### Invalid Sample Count

Test sample counts below 1 and above 100 and verify that the program rejects the invalid values.

## Known Limitations

This project is a software simulation and does not communicate with a physical temperature sensor.

The readings are deterministic and do not represent real environmental measurements. The simulator does not model sensor noise, calibration errors, sensor drift, or environmental changes.

The thresholds and operating modes are also fixed in the program and cannot currently be changed by the user.

## Author / Date

**Author:** Iman-Louise Mwai
**Date:** October 2026
