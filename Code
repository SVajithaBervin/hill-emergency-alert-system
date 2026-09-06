/*******************************************************
 * Intelligent Edge-Based Priority Communication
 * and Real-Time Emergency Analytics System
 *
 * Hill Area Emergency Alert System
 * ESP32 + DHT11 + Rain Sensor + Vibration Sensor
 * + SOS Button + LCD + LED + Buzzer
 * + Blynk IoT + ThingSpeak
 *
 * -----------------------------------------------------
 * IMPORTANT:
 * This is a RECONSTRUCTED REFERENCE IMPLEMENTATION
 * based on the documented project architecture.
 *
 * It is NOT claimed to be the exact original
 * historical source code used during the project.
 * -----------------------------------------------------
 *******************************************************/

// -------------------------
// Blynk Configuration
// -------------------------
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Hill Emergency Alert System"
#define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>
#include <ThingSpeak.h>

// =====================================================
// WiFi Configuration
// =====================================================

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// =====================================================
// ThingSpeak Configuration
// =====================================================

unsigned long thingspeakChannelNumber = YOUR_CHANNEL_ID;

const char* thingspeakApiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";

WiFiClient thingspeakClient;

// =====================================================
// Sensor Pin Configuration
// =====================================================

// DHT11
#define DHT_PIN 4
#define DHT_TYPE DHT11

// FC-37 Rain Sensor
#define RAIN_SENSOR_PIN 34

// SW-420 Vibration Sensor
#define VIBRATION_SENSOR_PIN 35

// Manual SOS Button
#define SOS_BUTTON_PIN 13

// =====================================================
// Output Pin Configuration
// =====================================================

#define RED_LED_PIN    25
#define YELLOW_LED_PIN 26
#define GREEN_LED_PIN  27

#define BUZZER_PIN 33

// =====================================================
// LCD Configuration
// =====================================================

#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS    2

LiquidCrystal_I2C lcd(
  LCD_ADDRESS,
  LCD_COLUMNS,
  LCD_ROWS
);

// =====================================================
// Sensor Object
// =====================================================

DHT dht(DHT_PIN, DHT_TYPE);

// =====================================================
// Blynk Timer
// =====================================================

BlynkTimer timer;

// =====================================================
// System Timing
// =====================================================

const unsigned long SENSOR_INTERVAL = 2000;
const unsigned long BLYNK_INTERVAL  = 10000;
const unsigned long THINGSPEAK_INTERVAL = 15000;

// =====================================================
// Sensor Values
// =====================================================

float temperature = 0.0;
float humidity = 0.0;

int rainRaw = 0;
float rainLevel = 0.0;

int vibrationState = 0;
int sosState = 0;

// =====================================================
// Priority System
// =====================================================

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

// =====================================================
// Alert Information
// =====================================================

String alertStatus = "SAFE";

bool buzzerActive = false;

unsigned long lastBuzzerChange = 0;

int buzzerStep = 0;

// =====================================================
// Rain Threshold
// =====================================================

const float HEAVY_RAIN_THRESHOLD = 70.0;

// =====================================================
// Temperature Threshold
// =====================================================

const float FIRE_RISK_TEMPERATURE = 40.0;

// =====================================================
// Function Prototypes
// =====================================================

void readSensors();

void classifyPriority();

void updateOutputs();

void updateLCD();

void sendBlynkData();

void sendThingSpeakData();

void handleBuzzer();

void stopBuzzer();

void setSafeOutputs();

void setCriticalOutputs();

void setWarningOutputs();

void sendAlertNotification();

float convertRainToPercentage(int rawValue);

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("======================================");
  Serial.println(" Hill Area Emergency Alert System");
  Serial.println(" ESP32 Emergency Monitoring Node");
  Serial.println("======================================");

  // -------------------------
  // Pin Configuration
  // -------------------------

  pinMode(RAIN_SENSOR_PIN, INPUT);

  pinMode(VIBRATION_SENSOR_PIN, INPUT);

  pinMode(SOS_BUTTON_PIN, INPUT_PULLUP);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  // -------------------------
  // Initial Outputs
  // -------------------------

  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  digitalWrite(BUZZER_PIN, LOW);

  // -------------------------
  // LCD Initialization
  // -------------------------

  Wire.begin();

  lcd.init();
  lcd.backlight();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Hill Emergency");

  lcd.setCursor(0, 1);
  lcd.print("System Starting");

  delay(2000);

  // -------------------------
  // DHT Initialization
  // -------------------------

  dht.begin();

  // -------------------------
  // WiFi + Blynk
  // -------------------------

  Serial.println("Connecting to WiFi...");

  Blynk.begin(
    BLYNK_AUTH_TOKEN,
    ssid,
    pass
  );

  Serial.println("Blynk connection initialized.");

  // -------------------------
  // ThingSpeak
  // -------------------------

  ThingSpeak.begin(thingspeakClient);

  Serial.println("ThingSpeak initialized.");

  // -------------------------
  // Timers
  // -------------------------

  timer.setInterval(
    SENSOR_INTERVAL,
    readSensors
  );

  timer.setInterval(
    BLYNK_INTERVAL,
    sendBlynkData
  );

  timer.setInterval(
    THINGSPEAK_INTERVAL,
    sendThingSpeakData
  );

  // -------------------------
  // First Sensor Reading
  // -------------------------

  readSensors();

  Serial.println("System Ready.");

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("System Ready");

  lcd.setCursor(0, 1);
  lcd.print("Monitoring...");
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
  Blynk.run();

  timer.run();

  handleBuzzer();
}

// =====================================================
// READ SENSORS
// =====================================================

void readSensors()
{
  // -------------------------
  // DHT11
  // -------------------------

  float newTemperature = dht.readTemperature();

  float newHumidity = dht.readHumidity();

  if (!isnan(newTemperature))
  {
    temperature = newTemperature;
  }

  if (!isnan(newHumidity))
  {
    humidity = newHumidity;
  }

  // -------------------------
  // Rain Sensor
  // -------------------------

  rainRaw = analogRead(RAIN_SENSOR_PIN);

  rainLevel = convertRainToPercentage(rainRaw);

  // -------------------------
  // Vibration Sensor
  // -------------------------

  vibrationState =
    digitalRead(VIBRATION_SENSOR_PIN);

  // -------------------------
  // SOS Button
  // -------------------------

  /*
   * INPUT_PULLUP is used.
   *
   * Button pressed  = LOW
   * Button released = HIGH
   */

  if (digitalRead(SOS_BUTTON_PIN) == LOW)
  {
    sosState = 1;
  }
  else
  {
    sosState = 0;
  }

  // -------------------------
  // Priority Classification
  // -------------------------

  classifyPriority();

  // -------------------------
  // Outputs
  // -------------------------

  updateOutputs();

  // -------------------------
  // LCD
  // -------------------------

  updateLCD();

  // -------------------------
  // Serial Monitor
  // -------------------------

  Serial.println();
  Serial.println("--------------------------------");

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity    : ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Rain Level  : ");
  Serial.print(rainLevel);
  Serial.println(" %");

  Serial.print("Vibration   : ");
  Serial.println(vibrationState);

  Serial.print("SOS         : ");
  Serial.println(sosState);

  Serial.print("Priority    : P");
  Serial.println(currentPriority);

  Serial.print("Alert       : ");
  Serial.println(alertStatus);

  Serial.println("--------------------------------");
}

// =====================================================
// RAIN CONVERSION
// =====================================================

float convertRainToPercentage(int rawValue)
{
  /*
   * Reference normalization for the analog rain input.
   *
   * IMPORTANT:
   * Actual FC-37 sensor polarity/calibration may vary.
   * Therefore this conversion should be calibrated
   * against the physical sensor before field deployment.
   */

  float percentage =
    100.0 -
    ((float)rawValue / 4095.0 * 100.0);

  if (percentage < 0)
  {
    percentage = 0;
  }

  if (percentage > 100)
  {
    percentage = 100;
  }

  return percentage;
}

// =====================================================
// PRIORITY CLASSIFICATION
// =====================================================

void classifyPriority()
{
  previousPriority = currentPriority;

  /*
   * Priority order:
   *
   * P1 = SOS
   * P2 = Landslide / Vibration
   * P3 = Heavy Rain
   * P4 = Fire Risk
   * P0 = Safe
   *
   * Human-triggered SOS always has the highest
   * priority over automatic sensor conditions.
   */

  if (sosState == 1)
  {
    currentPriority = PRIORITY_SOS;

    alertStatus = "SOS! HELP!";
  }

  else if (vibrationState == HIGH)
  {
    currentPriority = PRIORITY_LANDSLIDE;

    alertStatus = "LANDSLIDE!";
  }

  else if (rainLevel > HEAVY_RAIN_THRESHOLD)
  {
    currentPriority = PRIORITY_HEAVY_RAIN;

    alertStatus = "HEAVY RAIN";
  }

  else if (temperature > FIRE_RISK_TEMPERATURE)
  {
    currentPriority = PRIORITY_FIRE_RISK;

    alertStatus = "FIRE RISK!";
  }

  else
  {
    currentPriority = PRIORITY_SAFE;

    alertStatus = "SAFE";
  }

  // Send notification only when
  // priority state changes.
  if (currentPriority != previousPriority)
  {
    sendAlertNotification();
  }
}

// =====================================================
// OUTPUT CONTROL
// =====================================================

void updateOutputs()
{
  switch (currentPriority)
  {
    // -------------------------
    // SAFE
    // -------------------------

    case PRIORITY_SAFE:

      setSafeOutputs();

      break;

    // -------------------------
    // SOS
    // -------------------------

    case PRIORITY_SOS:

      setCriticalOutputs();

      break;

    // -------------------------
    // LANDSLIDE
    // -------------------------

    case PRIORITY_LANDSLIDE:

      setCriticalOutputs();

      break;

    // -------------------------
    // HEAVY RAIN
    // -------------------------

    case PRIORITY_HEAVY_RAIN:

      setWarningOutputs();

      break;

    // -------------------------
    // FIRE RISK
    // -------------------------

    case PRIORITY_FIRE_RISK:

      setWarningOutputs();

      break;

    default:

      setSafeOutputs();

      break;
  }
}

// =====================================================
// SAFE OUTPUT
// =====================================================

void setSafeOutputs()
{
  digitalWrite(RED_LED_PIN, LOW);

  digitalWrite(YELLOW_LED_PIN, LOW);

  digitalWrite(GREEN_LED_PIN, HIGH);

  stopBuzzer();
}

// =====================================================
// CRITICAL OUTPUT
// =====================================================

void setCriticalOutputs()
{
  digitalWrite(RED_LED_PIN, HIGH);

  digitalWrite(YELLOW_LED_PIN, LOW);

  digitalWrite(GREEN_LED_PIN, LOW);

  buzzerActive = true;
}

// =====================================================
// WARNING OUTPUT
// =====================================================

void setWarningOutputs()
{
  digitalWrite(RED_LED_PIN, LOW);

  digitalWrite(YELLOW_LED_PIN, HIGH);

  digitalWrite(GREEN_LED_PIN, LOW);

  buzzerActive = true;
}

// =====================================================
// BUZZER HANDLER
// =====================================================

void handleBuzzer()
{
  unsigned long currentTime = millis();

  if (!buzzerActive)
  {
    digitalWrite(BUZZER_PIN, LOW);

    return;
  }

  // -------------------------------------
  // Critical Alerts
  // -------------------------------------

  if (
    currentPriority == PRIORITY_SOS ||
    currentPriority == PRIORITY_LANDSLIDE
  )
  {
    /*
     * Five fast beeps.
     */

    if (currentTime - lastBuzzerChange >= 150)
    {
      lastBuzzerChange = currentTime;

      if (buzzerStep < 10)
      {
        digitalWrite(
          BUZZER_PIN,
          buzzerStep % 2 == 0 ? HIGH : LOW
        );

        buzzerStep++;
      }
      else
      {
        buzzerStep = 0;

        digitalWrite(
          BUZZER_PIN,
          LOW
        );
      }
    }
  }

  // -------------------------------------
  // Warning Alerts
  // -------------------------------------

  else
  {
    /*
     * Two longer warning beeps.
     */

    if (currentTime - lastBuzzerChange >= 500)
    {
      lastBuzzerChange = currentTime;

      if (buzzerStep < 4)
      {
        digitalWrite(
          BUZZER_PIN,
          buzzerStep % 2 == 0 ? HIGH : LOW
        );

        buzzerStep++;
      }
      else
      {
        buzzerStep = 0;

        digitalWrite(
          BUZZER_PIN,
          LOW
        );
      }
    }
  }
}

// =====================================================
// STOP BUZZER
// =====================================================

void stopBuzzer()
{
  buzzerActive = false;

  buzzerStep = 0;

  digitalWrite(
    BUZZER_PIN,
    LOW
  );
}

// =====================================================
// LCD UPDATE
// =====================================================

void updateLCD()
{
  lcd.clear();

  // -------------------------
  // First Line
  // -------------------------

  lcd.setCursor(0, 0);

  if (currentPriority == PRIORITY_SAFE)
  {
    lcd.print("SAFE");
  }

  else if (currentPriority == PRIORITY_SOS)
  {
    lcd.print("SOS! HELP!");
  }

  else if (currentPriority == PRIORITY_LANDSLIDE)
  {
    lcd.print("LANDSLIDE!");
  }

  else if (currentPriority == PRIORITY_HEAVY_RAIN)
  {
    lcd.print("HEAVY RAIN");
  }

  else if (currentPriority == PRIORITY_FIRE_RISK)
  {
    lcd.print("FIRE RISK!");
  }

  // -------------------------
  // Second Line
  // -------------------------

  lcd.setCursor(0, 1);

  lcd.print("P");
  lcd.print(currentPriority);

  lcd.print(" T:");
  lcd.print(temperature, 1);

  lcd.print("C");
}

// =====================================================
// BLYNK DATA
// =====================================================

void sendBlynkData()
{
  /*
   * Blynk virtual datastream mapping:
   *
   * V0 = Temperature
   * V1 = Humidity
   * V2 = Rain Level
   * V3 = Vibration
   * V4 = Alert Status
   */

  Blynk.virtualWrite(
    V0,
    temperature
  );

  Blynk.virtualWrite(
    V1,
    humidity
  );

  Blynk.virtualWrite(
    V2,
    rainLevel
  );

  Blynk.virtualWrite(
    V3,
    vibrationState
  );

  Blynk.virtualWrite(
    V4,
    alertStatus
  );

  Serial.println(
    "Blynk data updated."
  );
}

// =====================================================
// THINGSPEAK DATA
// =====================================================

void sendThingSpeakData()
{
  /*
   * ThingSpeak fields:
   *
   * Field 1 = Temperature
   * Field 2 = Humidity
   * Field 3 = Rain Level
   * Field 4 = Vibration
   */

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(
      "ThingSpeak: WiFi not connected."
    );

    return;
  }

  ThingSpeak.setField(
    1,
    temperature
  );

  ThingSpeak.setField(
    2,
    humidity
  );

  ThingSpeak.setField(
    3,
    rainLevel
  );

  ThingSpeak.setField(
    4,
    vibrationState
  );

  int responseCode =
    ThingSpeak.writeFields(
      thingspeakChannelNumber,
      thingspeakApiKey
    );

  if (responseCode == 200)
  {
    Serial.println(
      "ThingSpeak update successful."
    );
  }
  else
  {
    Serial.print(
      "ThingSpeak update failed. Code: "
    );

    Serial.println(
      responseCode
    );
  }
}

// =====================================================
// BLYNK ALERT NOTIFICATION
// =====================================================

void sendAlertNotification()
{
  /*
   * Blynk Event Codes:
   *
   * sos_alert
   * landslide_alert
   * heavy_rain_alert
   * fire_risk_alert
   */

  if (currentPriority == PRIORITY_SOS)
  {
    Blynk.logEvent(
      "sos_alert",
      "SOS! Emergency button activated."
    );
  }

  else if (currentPriority == PRIORITY_LANDSLIDE)
  {
    Blynk.logEvent(
      "landslide_alert",
      "High vibration detected. Possible landslide."
    );
  }

  else if (currentPriority == PRIORITY_HEAVY_RAIN)
  {
    Blynk.logEvent(
      "heavy_rain_alert",
      "Heavy rain condition detected."
    );
  }

  else if (currentPriority == PRIORITY_FIRE_RISK)
  {
    Blynk.logEvent(
      "fire_risk_alert",
      "High temperature / fire-risk condition detected."
    );
  }

  Serial.print(
    "Alert event triggered: "
  );

  Serial.println(
    alertStatus
  );
}
