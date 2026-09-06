# Intelligent Edge-Based Priority Communication and Real-Time Emergency Analytics System

### Hill Area Emergency Alert System using ESP32, Blynk & ThingSpeak

A low-cost IoT-based emergency alert prototype designed to demonstrate priority-based detection and local alerting for disaster-related conditions in remote and hill-area environments.

---

## Overview

Remote hill areas can face risks such as landslides, heavy rainfall and other emergencies where early detection and rapid alerting can be valuable.

This project demonstrates an ESP32-based sensing and alert prototype that monitors environmental and emergency inputs and classifies conditions according to predefined priority levels.

The system provides:

- Local visual alerts using LEDs
- Audible alerts using a buzzer
- Status display using a 16x2 I2C LCD
- Sensor monitoring using ESP32
- Blynk IoT integration
- ThingSpeak data logging
- Wokwi-based simulation
- Priority-based emergency classification

---

## System Architecture

The project is organized around three functional layers:

1. Edge Layer
2. Communication Layer
3. Cloud Monitoring Layer

### Edge Layer

The ESP32 acts as the main controller and processes sensor inputs locally.

### Communication Layer

Wi-Fi was used for prototype/simulation connectivity.

LoRaWAN is considered as a future deployment option for remote areas where conventional connectivity may not be available.

### Cloud Layer

Blynk is used for IoT monitoring and alert visualization.

ThingSpeak is used for sensor data logging and time-series visualization.

---

## Hardware Components

| Component | Purpose |
|---|---|
| ESP32 WROOM-32 | Main controller |
| DHT11 | Temperature and humidity |
| FC-37 Rain Sensor | Rain-level input |
| SW-420 Vibration Sensor | Vibration/landslide-related input |
| SOS Push Button | Manual emergency trigger |
| Red LED | Critical alert |
| Yellow LED | Warning alert |
| Green LED | Safe condition |
| Piezo Buzzer | Audible alert |
| 16x2 I2C LCD | Local status display |
| EE1040 Power Module | Power regulation |

---

## Priority-Based Alert Logic

| Priority | Condition | Response |
|---|---|---|
| P1 | SOS button pressed | Red LED + fast buzzer |
| P2 | High vibration | Red LED + fast buzzer |
| P3 | Heavy rain | Yellow LED + warning buzzer |
| P4 | High temperature / fire-risk threshold | Yellow LED + warning buzzer |
| P0 | Normal condition | Green LED + buzzer OFF |

The priority logic ensures that a manual SOS event takes precedence over other automated conditions.

---

## Blynk Integration

The prototype architecture uses Blynk for monitoring:

- Temperature
- Humidity
- Rain level
- Vibration
- Alert status

The system can generate alert events for non-safe conditions.

---

## ThingSpeak Integration

ThingSpeak is used for sensor-data logging and time-series visualization.

The documented channel structure contains four sensor fields:

1. Temperature
2. Humidity
3. Rain Level
4. Vibration

---

## Wokwi Simulation

Wokwi is used to demonstrate the circuit structure and system logic.

The Wokwi implementation may use simulation-compatible substitute components where the exact physical component is not available.

For example, the physical prototype uses DHT11, while a DHT22 may be used as a simulation substitute.

---

## Hardware Testing

The physical prototype was assembled and tested at hardware level.

Functional checks included:

- Safe condition
- Landslide/vibration alert
- Heavy rain alert
- SOS alert
- Blynk connectivity
- ThingSpeak logging

The hardware tests focused on verifying the programmed response of the prototype to controlled/test sensor inputs.

---

## Important Testing Note

The prototype was not deployed in an actual hill location for full environmental field-data collection.

The demonstrated hardware tests used controlled/programmed test conditions to verify the ESP32 alert logic and output behaviour.

Therefore, the readings shown in simulation or controlled demonstrations should not be interpreted as long-term real-world hill-area environmental measurements.

---

## Current Limitation

Actual soil-based vibration testing and field deployment in a hill environment were not completed during the initial project presentation.

A future field version would require:

- Suitable soil/ground vibration sensing
- Protective enclosure
- Water and dust protection
- Outdoor wiring protection
- Long-duration testing
- Actual hill-area data collection

---

## Future Scope

Potential future enhancements include:

- Soil/ground vibration sensing
- Waterproof and rugged enclosure
- LoRaWAN communication
- GPS-based alert location
- GSM/SMS fallback
- Solar and Li-Po power
- Long-term field data collection
- Machine-learning-based predictive analysis

---

## Project Status

**Prototype / Demonstration Stage**

The project demonstrates the hardware architecture, priority-based alert logic, IoT monitoring and simulation workflow.

Actual hill-area field validation remains a future enhancement.

---

## Repository Structure

```text
code/        -> ESP32 reference and Wokwi firmware
wokwi/       -> Wokwi simulation files
docs/        -> Project documentation
diagrams/    -> Block and architecture diagrams
screenshots/ -> Actual/reference output images
media/       -> Supporting project documents
