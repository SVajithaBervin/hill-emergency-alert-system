/************************************************************
 * Intelligent Edge-Based Priority Communication
 * and Real-Time Emergency Analytics System
 *
 * Hill Area Emergency Alert System
 *
 * WOKWI SIMULATION VERSION
 *
 * Physical Project:
 *   ESP32 + DHT11 + FC-37 Rain Sensor
 *   + SW-420 Vibration Sensor + SOS Button
 *   + LCD + LEDs + Buzzer
 *
 * Wokwi Simulation:
 *   ESP32 + DHT22 + Potentiometer
 *   + Pushbutton Vibration Input + SOS Button
 *   + LCD + LEDs + Buzzer
 *
 * ----------------------------------------------------------
 * IMPORTANT
 * ----------------------------------------------------------
 * DHT22 is used ONLY as a Wokwi simulation substitute
 * for the physical DHT11 used in the project.
 *
 * The potentiometer is used as a controllable simulation
 * input for the rain sensor.
 *
 * The VIBRATION pushbutton is used as a controllable
 * simulation input for the SW-420 vibration sensor.
 *
 * This file is a RECONSTRUCTED REFERENCE IMPLEMENTATION.
 * It is not claimed to be the exact original competition
 * source code.
 ************************************************************/

#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// ==========================================================
// OPTIONAL CLOUD INTEGRATION
// ==========================================================
//
// Set this to 1 only after adding valid Blynk and
// ThingSpeak credentials.
//
// Default = 0
//
// This allows the Wokwi simulation to demonstrate the
// complete local alert logic without requiring private
// cloud credentials.
//

#define ENABLE_CLOUD_INTEGRATION 0

#if ENABLE_CLOUD_INTEGRATION

#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Hill Emergency Alert System"
#define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ThingSpeak.h>

char wifiSSID[] = "YOUR_WIFI_NAME";
char wifiPassword[] = "YOUR_WIFI_PASSWORD";

unsigned long thingSpeakChannel =
  YOUR_THINGSPEAK_CHANNEL_ID;

const char* thingSpeakWriteAPIKey =
  "YOUR_THINGSPEAK_WRITE_API_KEY";

WiFiClient thingSpeakClient;

BlynkTimer cloudTimer;

#endif

// ==========================================================
// SENSOR CONFIGURATION
// ==========================================================

// Wokwi DHT22
// Physical project uses DHT11.
#define DHT_PIN 4
#define DHT_TYPE DHT22

// Physical project:
// FC-37 Rain Sensor -> GPIO34
//
// Wokwi:
// Potentiometer -> GPIO34
#define RAIN_SENSOR_PIN 34

// Physical project:
// SW-420 Vibration Sensor -> GPIO35
//
// Wokwi:
// Pushbutton -> GPIO35
#define VIBRATION_SENSOR_PIN 35

// Manual emergency button
#define SOS_BUTTON_PIN 13

// ==========================================================
// OUTPUT CONFIGURATION
// ==========================================================

#define RED_LED_PIN    25
#define YELLOW_LED_PIN 26
#define GREEN_LED_PIN  27

#define BUZZER_PIN 33

// ==========================================================
// LCD CONFIGURATION
// ==========================================================

#define LCD_ADDRESS 0x27

LiquidCrystal_I2C lcd(
  LCD_ADDRESS,
  16,
  2
);

// ==========================================================
// DHT SENSOR
// ==========================================================

DHT dht(
  DHT_PIN,
  DHT_TYPE
);

// ==========================================================
// SENSOR VALUES
// ==========================================================

float temperature = 28.0;
float humidity = 62.0;

int rainRaw = 0;

float rainLevel = 10.0;

int vibrationState = 0;

int sosState = 0;

// ==========================================================
// PRIORITY DEFINITIONS
// ==========================================================

/*
 * Priority structure based on the project documentation:
 *
 * P0 = SAFE
 * P1 = SOS
 * P2 = LANDSLIDE / VIBRATION
 * P3 = HEAVY RAIN
 * P4 = FIRE RISK
 */

enum PriorityLevel
{
  PRIORITY_SAFE = 0,
  PRIORITY_SOS = 1,
  PRIORITY_LANDSLIDE = 2,
  PRIORITY_HEAVY_RAIN = 3,
  PRIORITY_FIRE_RISK = 4
};

int currentPriority = PRIORITY_SAFE;

int previousPriority = PRIORITY_SAFE;

// ==========================================================
// ALERT STATUS
// ==========================================================

String alertStatus = "SAFE";

// ==========================================================
// THRESHOLDS
// ==========================================================

const float HEAVY_RAIN_THRESHOLD = 70.0;

const float FIRE_RISK_THRESHOLD = 40.0;

// ==========================================================
// TIMING
// ==========================================================

unsigned long lastSensorRead = 0;

unsigned long lastLCDUpdate = 0;

unsigned long lastSerialPrint = 0;

unsigned long lastBuzzerChange = 0;

// Sensor interval
const unsigned long SENSOR_INTERVAL = 1000;

// LCD interval
const unsigned long LCD_INTERVAL = 500;

// Serial interval
const unsigned long SERIAL_INTERVAL = 1000;

// ==========================================================
// BUZZER STATE
// ==========================================================

bool buzzerActive = false;

bool buzzerOutputState = false;

int buzzerStep = 0;

// ==========================================================
// SETUP
// ==========================================================

void setup()
{
  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("==============================================");
  Serial.println(" HILL AREA EMERGENCY ALERT SYSTEM");
  Serial.println(" WOKWI REFERENCE SIMULATION");
  Serial.println("==============================================");

  // --------------------------------------------------------
  // GPIO configuration
  // --------------------------------------------------------

  pinMode(
    RAIN_SENSOR_PIN,
    INPUT
  );

  pinMode(
    VIBRATION_SENSOR_PIN,
    INPUT
  );

  pinMode(
    SOS_BUTTON_PIN,
    INPUT_PULLUP
  );

  pinMode(
    RED_LED_PIN,
    OUTPUT
  );

  pinMode(
    YELLOW_LED_PIN,
    OUTPUT
  );

  pinMode(
    GREEN_LED_PIN,
    OUTPUT
  );

  pinMode(
    BUZZER_PIN,
    OUTPUT
  );

  // --------------------------------------------------------
  // Initial output state
  // --------------------------------------------------------

  digitalWrite(
    RED_LED_PIN,
    LOW
  );

  digitalWrite(
    YELLOW_LED_PIN,
    LOW
  );

  digitalWrite(
    GREEN_LED_PIN,
    LOW
  );

  digitalWrite(
    BUZZER_PIN,
    LOW
  );

  // --------------------------------------------------------
  // I2C / LCD
  // --------------------------------------------------------

  Wire.begin(
    21,
    22
  );

  lcd.init();

  lcd.backlight();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Hill Emergency");

  lcd.setCursor(0, 1);
  lcd.print("Wokwi Starting");

  // --------------------------------------------------------
  // DHT
  // --------------------------------------------------------

  dht.begin();

  delay(1500);

  // --------------------------------------------------------
  // Cloud initialization
  // --------------------------------------------------------

#if ENABLE_CLOUD_INTEGRATION

  Serial.println(
    "Cloud integration enabled."
  );

  Blynk.begin(
    BLYNK_AUTH_TOKEN,
    wifiSSID,
    wifiPassword
  );

  ThingSpeak.begin(
    thingSpeakClient
  );

#else

  Serial.println(
    "Cloud integration disabled for local simulation."
  );

  Serial.println(
    "Blynk/ThingSpeak output is represented in Serial Monitor."
  );

#endif

  // --------------------------------------------------------
  // Initial system state
  // --------------------------------------------------------

  readSensors();

  classifyPriority();

  updateOutputs();

  updateLCD();

  Serial.println();
  Serial.println(
    "Simulation Ready."
  );

  Serial.println(
    "Controls:"
  );

  Serial.println(
    "  V = Vibration button"
  );

  Serial.println(
    "  S = SOS button"
  );

  Serial.println(
    "  Click DHT22 to change temperature/humidity"
  );

  Serial.println(
    "  Turn potentiometer to change rain level"
  );

  Serial.println(
    "==============================================");
}

// ==========================================================
// MAIN LOOP
// ==========================================================

void loop()
{
#if ENABLE_CLOUD_INTEGRATION

  Blynk.run();

#endif

  unsigned long now = millis();

  // --------------------------------------------------------
  // Sensor processing
  // --------------------------------------------------------

  if (
    now - lastSensorRead >=
    SENSOR_INTERVAL
  )
  {
    lastSensorRead = now;

    readSensors();

    classifyPriority();

    updateOutputs();
  }

  // --------------------------------------------------------
  // LCD update
  // --------------------------------------------------------

  if (
    now - lastLCDUpdate >=
    LCD_INTERVAL
  )
  {
    lastLCDUpdate = now;

    updateLCD();
  }

  // --------------------------------------------------------
  // Serial output
  // --------------------------------------------------------

  if (
    now - lastSerialPrint >=
    SERIAL_INTERVAL
  )
  {
    lastSerialPrint = now;

    printSerialData();
  }

  // --------------------------------------------------------
  // Buzzer
  // --------------------------------------------------------

  handleBuzzer();

#if ENABLE_CLOUD_INTEGRATION

  cloudTimer.run();

#endif
}

// ==========================================================
// READ SENSOR VALUES
// ==========================================================

void readSensors()
{
  // --------------------------------------------------------
  // DHT22
  // --------------------------------------------------------

  float newTemperature =
    dht.readTemperature();

  float newHumidity =
    dht.readHumidity();

  if (!isnan(newTemperature))
  {
    temperature =
      newTemperature;
  }

  if (!isnan(newHumidity))
  {
    humidity =
      newHumidity;
  }

  // --------------------------------------------------------
  // Rain simulation
  // --------------------------------------------------------

  rainRaw =
    analogRead(
      RAIN_SENSOR_PIN
    );

  /*
   * Wokwi potentiometer is used as a controllable
   * substitute for the physical FC-37 rain sensor.
   *
   * The mapping is intentionally simulation-oriented.
   *
   * Potentiometer:
   *   low position  -> high rain level
   *   high position -> low rain level
   */

  rainLevel =
    calculateRainLevel(
      rainRaw
    );

  // --------------------------------------------------------
  // Vibration
  // --------------------------------------------------------

  /*
   * External pull-up resistor is used in diagram.json.
   *
   * Button pressed:
   *   LOW
   *
   * Button released:
   *   HIGH
   */

  if (
    digitalRead(
      VIBRATION_SENSOR_PIN
    ) == LOW
  )
  {
    vibrationState = 1;
  }
  else
  {
    vibrationState = 0;
  }

  // --------------------------------------------------------
  // SOS
  // --------------------------------------------------------

  if (
    digitalRead(
      SOS_BUTTON_PIN
    ) == LOW
  )
  {
    sosState = 1;
  }
  else
  {
    sosState = 0;
  }
}

// ==========================================================
// RAIN LEVEL CALCULATION
// ==========================================================

float calculateRainLevel(
  int rawValue
)
{
  /*
   * Wokwi's potentiometer control ranges from 0 to 1023.
   *
   * The ESP32 ADC may expose the value on a larger
   * numeric scale, so we normalize using the observed
   * maximum range.
   */

  const float ADC_MAX = 4095.0;

  float normalized =
    (float)rawValue /
    ADC_MAX;

  /*
   * If the simulator provides a lower ADC range,
   * normalize it into the expected 0-1 range.
   */

  if (normalized > 1.0)
  {
    normalized = 1.0;
  }

  /*
   * Low raw value = higher simulated rain.
   * High raw value = lower simulated rain.
   */

  float level =
    (1.0 - normalized) *
    100.0;

  if (level < 0)
  {
    level = 0;
  }

  if (level > 100)
  {
    level = 100;
  }

  return level;
}

// ==========================================================
// PRIORITY CLASSIFICATION
// ==========================================================

void classifyPriority()
{
  previousPriority =
    currentPriority;

  /*
   * Highest priority first.
   *
   * P1 - SOS
   * P2 - Vibration / Landslide
   * P3 - Heavy Rain
   * P4 - Fire Risk
   * P0 - Safe
   */

  if (sosState == 1)
  {
    currentPriority =
      PRIORITY_SOS;

    alertStatus =
      "SOS! HELP!";
  }

  else if (vibrationState == 1)
  {
    currentPriority =
      PRIORITY_LANDSLIDE;

    alertStatus =
      "LANDSLIDE!";
  }

  else if (
    rainLevel >
    HEAVY_RAIN_THRESHOLD
  )
  {
    currentPriority =
      PRIORITY_HEAVY_RAIN;

    alertStatus =
      "HEAVY RAIN";
  }

  else if (
    temperature >
    FIRE_RISK_THRESHOLD
  )
  {
    currentPriority =
      PRIORITY_FIRE_RISK;

    alertStatus =
      "FIRE RISK!";
  }

  else
  {
    currentPriority =
      PRIORITY_SAFE;

    alertStatus =
      "SAFE";
  }

  // --------------------------------------------------------
  // Trigger notification when state changes
  // --------------------------------------------------------

  if (
    currentPriority !=
    previousPriority
  )
  {
    onPriorityChanged();
  }
}

// ==========================================================
// PRIORITY CHANGE HANDLER
// ==========================================================

void onPriorityChanged()
{
  Serial.println();
  Serial.println(
    "******** PRIORITY CHANGE ********"
  );

  Serial.print(
    "New Priority: P"
  );

  Serial.println(
    currentPriority
  );

  Serial.print(
    "Alert: "
  );

  Serial.println(
    alertStatus
  );

  Serial.println(
    "*********************************"
  );

#if ENABLE_CLOUD_INTEGRATION

  sendBlynkAlert();

#endif
}

// ==========================================================
// OUTPUT CONTROL
// ==========================================================

void updateOutputs()
{
  switch (
    currentPriority
  )
  {
    case PRIORITY_SAFE:

      setSafeOutputs();

      break;

    case PRIORITY_SOS:

      setCriticalOutputs();

      break;

    case PRIORITY_LANDSLIDE:

      setCriticalOutputs();

      break;

    case PRIORITY_HEAVY_RAIN:

      setWarningOutputs();

      break;

    case PRIORITY_FIRE_RISK:

      setWarningOutputs();

      break;

    default:

      setSafeOutputs();

      break;
  }
}

// ==========================================================
// SAFE OUTPUTS
// ==========================================================

void setSafeOutputs()
{
  digitalWrite(
    RED_LED_PIN,
    LOW
  );

  digitalWrite(
    YELLOW_LED_PIN,
    LOW
  );

  digitalWrite(
    GREEN_LED_PIN,
    HIGH
  );

  buzzerActive =
    false;

  buzzerStep =
    0;

  digitalWrite(
    BUZZER_PIN,
    LOW
  );
}

// ==========================================================
// CRITICAL OUTPUTS
// ==========================================================

void setCriticalOutputs()
{
  digitalWrite(
    RED_LED_PIN,
    HIGH
  );

  digitalWrite(
    YELLOW_LED_PIN,
    LOW
  );

  digitalWrite(
    GREEN_LED_PIN,
    LOW
  );

  buzzerActive =
    true;
}

// ==========================================================
// WARNING OUTPUTS
// ==========================================================

void setWarningOutputs()
{
  digitalWrite(
    RED_LED_PIN,
    LOW
  );

  digitalWrite(
    YELLOW_LED_PIN,
    HIGH
  );

  digitalWrite(
    GREEN_LED_PIN,
    LOW
  );

  buzzerActive =
    true;
}

// ==========================================================
// BUZZER HANDLER
// ==========================================================

void handleBuzzer()
{
  if (!buzzerActive)
  {
    digitalWrite(
      BUZZER_PIN,
      LOW
    );

    return;
  }

  unsigned long now =
    millis();

  // --------------------------------------------------------
  // Critical alert
  // --------------------------------------------------------

  if (
    currentPriority ==
    PRIORITY_SOS ||

    currentPriority ==
    PRIORITY_LANDSLIDE
  )
  {
    /*
     * Five fast beeps.
     *
     * Each complete cycle:
     *   HIGH
     *   LOW
     */

    if (
      now -
      lastBuzzerChange >=
      150
    )
    {
      lastBuzzerChange =
        now;

      if (
        buzzerStep <
        10
      )
      {
        buzzerOutputState =
          !buzzerOutputState;

        digitalWrite(
          BUZZER_PIN,
          buzzerOutputState
        );

        buzzerStep++;
      }

      else
      {
        buzzerStep =
          0;

        buzzerOutputState =
          false;

        digitalWrite(
          BUZZER_PIN,
          LOW
        );
      }
    }
  }

  // --------------------------------------------------------
  // Warning alert
  // --------------------------------------------------------

  else
  {
    /*
     * Two longer warning beeps.
     */

    if (
      now -
      lastBuzzerChange >=
      500
    )
    {
      lastBuzzerChange =
        now;

      if (
        buzzerStep <
        4
      )
      {
        buzzerOutputState =
          !buzzerOutputState;

        digitalWrite(
          BUZZER_PIN,
          buzzerOutputState
        );

        buzzerStep++;
      }

      else
      {
        buzzerStep =
          0;

        buzzerOutputState =
          false;

        digitalWrite(
          BUZZER_PIN,
          LOW
        );
      }
    }
  }
}

// ==========================================================
// LCD UPDATE
// ==========================================================

void updateLCD()
{
  lcd.clear();

  // --------------------------------------------------------
  // Line 1: Alert
  // --------------------------------------------------------

  lcd.setCursor(
    0,
    0
  );

  if (
    currentPriority ==
    PRIORITY_SAFE
  )
  {
    lcd.print(
      "SAFE"
    );
  }

  else if (
    currentPriority ==
    PRIORITY_SOS
  )
  {
    lcd.print(
      "SOS! HELP!"
    );
  }

  else if (
    currentPriority ==
    PRIORITY_LANDSLIDE
  )
  {
    lcd.print(
      "LANDSLIDE!"
    );
  }

  else if (
    currentPriority ==
    PRIORITY_HEAVY_RAIN
  )
  {
    lcd.print(
      "HEAVY RAIN"
    );
  }

  else if (
    currentPriority ==
    PRIORITY_FIRE_RISK
  )
  {
    lcd.print(
      "FIRE RISK!"
    );
  }

  // --------------------------------------------------------
  // Line 2: Priority + temperature
  // --------------------------------------------------------

  lcd.setCursor(
    0,
    1
  );

  lcd.print(
    "P"
  );

  lcd.print(
    currentPriority
  );

  lcd.print(
    " T:"
  );

  lcd.print(
    temperature,
    1
  );

  lcd.print(
    "C"
  );
}

// ==========================================================
// SERIAL MONITOR
// ==========================================================

void printSerialData()
{
  Serial.println();
  Serial.println(
    "------------------------------------------"
  );

  Serial.print(
    "Temperature : "
  );

  Serial.print(
    temperature,
    1
  );

  Serial.println(
    " C"
  );

  Serial.print(
    "Humidity    : "
  );

  Serial.print(
    humidity,
    1
  );

  Serial.println(
    " %"
  );

  Serial.print(
    "Rain Level  : "
  );

  Serial.print(
    rainLevel,
    1
  );

  Serial.println(
    " %"
  );

  Serial.print(
    "Vibration   : "
  );

  Serial.println(
    vibrationState
  );

  Serial.print(
    "SOS         : "
  );

  Serial.println(
    sosState
  );

  Serial.print(
    "Priority    : P"
  );

  Serial.println(
    currentPriority
  );

  Serial.print(
    "Alert       : "
  );

  Serial.println(
    alertStatus
  );

  Serial.println(
    "------------------------------------------"
  );

  // -----------------------------
