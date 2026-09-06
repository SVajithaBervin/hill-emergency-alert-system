# Hardware Components

## ESP32 WROOM-32

Acts as the primary microcontroller and processes sensor inputs.

## DHT11

Used for temperature and humidity monitoring.

Configured temperature input:
GPIO 4

## FC-37 Rain Sensor

Used to detect rainfall level.

Analog input:
GPIO 34

## SW-420 Vibration Sensor

Used to detect significant vibration events associated with the landslide-alert concept.

Digital input:
GPIO 35

## Manual SOS Push Button

Provides a manually triggered emergency input.

GPIO:
13

## Tri-Colour LEDs

Used to indicate system status:

- Green – Safe
- Yellow – High-priority warning
- Red – Critical alert

## Piezo Buzzer

Provides audible alerts according to the detected priority.

## 16×2 I2C LCD

Displays system status and alert information locally.

## EE1040 Power Module

Used for power regulation for the prototype.
