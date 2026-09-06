# System Architecture

## Functional Flow

```text
Sensors / Inputs
      |
      v
ESP32 Edge Controller
      |
      v
Read + Normalize Inputs
      |
      v
Priority Classification
      |
      +-----------------------------+
      |                             |
      v                             v
Local Alert Outputs             IoT Monitoring
      |                             |
      +-- LCD                        +-- Blynk
      +-- LEDs                       +-- ThingSpeak
      +-- Buzzer
```

## Inputs

| Input | Physical component | Reference GPIO |
|---|---|---:|
| Temperature / humidity | DHT11 | GPIO 4 |
| Rain | FC-37 analog output | GPIO 34 |
| Vibration | SW-420 digital output | GPIO 35 |
| Emergency SOS | Push button | GPIO 13 |

## Outputs

| Output | GPIO/interface | Purpose |
|---|---|---|
| Red LED | GPIO 25 | Critical alert |
| Yellow LED | GPIO 26 | Warning |
| Green LED | GPIO 27 | Safe |
| Buzzer | GPIO 33 | Audible alert |
| LCD 16×2 | I2C | Local status display |

I2C: SDA GPIO 21, SCL GPIO 22, address 0x27.

## Decision Logic

```text
IF SOS active
    P1 / SOS EMERGENCY
ELSE IF vibration active
    P2 / LANDSLIDE
ELSE IF rain > 70%
    P3 / HEAVY RAIN
ELSE IF temperature > 40 C
    P4 / FIRE RISK
ELSE
    P0 / SAFE
```

## Cloud Data

### Blynk

```text
V0 -> Temperature
V1 -> Humidity
V2 -> Rain Level
V3 -> Vibration
V4 -> Alert Status / Priority
```

### ThingSpeak

```text
Field 1 -> Temperature
Field 2 -> Humidity
Field 3 -> Rain Level
Field 4 -> Vibration
```

## Architecture Boundary

The local ESP32 alert logic is the core prototype behavior. Cloud services are an additional monitoring/data layer. The documented deployment concept identifies LoRaWAN as a future communication option where Wi-Fi is unsuitable.
