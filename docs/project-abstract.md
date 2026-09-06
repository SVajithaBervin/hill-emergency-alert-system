# Project Abstract

## Intelligent Edge-Based Priority Communication and Real-Time Emergency Analytics System

Remote hill communities are vulnerable to natural hazards such as landslides, heavy rainfall and other emergency conditions. Communication infrastructure may become unreliable during severe weather events, creating difficulties in delivering timely warnings and information to residents and rescue teams.

This project presents a low-cost IoT-based emergency detection and monitoring system using an ESP32 microcontroller and multiple environmental sensors. The system monitors temperature, rainfall, vibration and manual SOS input to identify potentially hazardous conditions.

An edge-based priority classification mechanism is implemented to assign different alert levels. Manual SOS is treated as the highest-priority event, followed by vibration-based landslide detection, heavy rainfall and high-temperature conditions.

The ESP32 generates local alerts using LEDs, a buzzer and an LCD display. For remote monitoring, the prototype integrates Blynk IoT for real-time notifications and ThingSpeak for sensor-data logging and time-series analysis.

The system demonstrates the feasibility of combining edge processing, IoT sensing and cloud analytics into a low-cost emergency monitoring solution for remote and hill-area environments.
