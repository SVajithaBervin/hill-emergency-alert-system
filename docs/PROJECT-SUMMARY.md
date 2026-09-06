# Project Summary

## Project Title

Intelligent Edge-Based Priority Communication and Real-Time Emergency Analytics System

### Subtitle

Hill Area Emergency Alert System using ESP32, Blynk & ThingSpeak

---

## Problem

Remote and hill-area communities may face hazards such as landslides and heavy rainfall, creating a need for rapid local detection and alerting.

The project explores a low-cost IoT-based approach using ESP32 and multiple sensors.

---

## Objective

The objective is to develop a prototype capable of:

1. Monitoring sensor inputs
2. Classifying emergency conditions
3. Assigning alert priorities
4. Generating local visual and audible alerts
5. Displaying status on an LCD
6. Sending/visualizing information through IoT platforms
7. Demonstrating the system using simulation

---

## Core Controller

ESP32 WROOM-32

---

## Inputs

- DHT11
- FC-37 Rain Sensor
- SW-420 Vibration Sensor
- SOS Push Button

---

## Outputs

- Red LED
- Yellow LED
- Green LED
- Piezo Buzzer
- 16x2 I2C LCD

---

## Cloud Platforms

### Blynk

Used for IoT monitoring and alert visualization.

### ThingSpeak

Used for sensor-data logging and time-series visualization.

---

## Simulation

Wokwi was used to demonstrate the circuit and software logic.

---

## Testing Approach

The physical prototype was tested using controlled/test inputs to verify whether the programmed logic generated the expected:

- LED response
- Buzzer response
- LCD message
- Alert status
- IoT output

The project was not field-tested in an actual hill environment during the initial presentation.

---

## Future Validation

The next stage is actual field testing with:

- Ground/soil vibration sensing
- Protective enclosure
- Outdoor deployment
- Real environmental data collection
- Long-duration monitoring
