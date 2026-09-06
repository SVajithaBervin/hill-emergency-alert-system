# Cloud Integration

## Blynk IoT

Blynk is used for real-time monitoring and notification.

The documented datastreams include:

1. Temperature
2. Humidity
3. Rain Level
4. Vibration
5. Alert Status

The project documentation specifies a 10-second update interval for the Blynk datastreams.

Non-safe conditions can trigger Blynk events/notifications.

## ThingSpeak

ThingSpeak is used for sensor-data logging and time-series visualization.

The project documentation specifies a 15-second logging interval.

The stored data can be used for retrospective analysis of environmental conditions.

## Important Implementation Note

Cloud communication requires network connectivity.

The local ESP32 alert mechanism is intended to operate at the edge, while Blynk and ThingSpeak provide remote/cloud monitoring when connectivity is available.
