// Blynk and Library Section
#define BLYNK_TEMPLATE_ID "TMPL666EqW8aD"
#define BLYNK_TEMPLATE_NAME "IoT Project"
#define BLYNK_AUTH_TOKEN "uBZj-A_paaaeospDaaen9jx9_gwCPaWZ"

#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pins
#define LED_PIN     4
#define Buzzer_PIN  18

// WiFi Credentials
char ssid[] = "Redmi11";
char pass[] = "passward";

// OLED Display
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// BMP280 Object: Help in Pressure Sensing
Adafruit_BMP280 bmp;

// Pressure Variables
float normal_temperature = 0;
const float base_psi = 14.7;      // Base Pressure
const float warning_appear = 14.5;    // Warning appears at this pressure
const float warning_disappears = 14.6;  // Warning disappears at this pressure
const float multiplier = 3.0;         // Helps in Pressure calculation

bool LOWwarning = false;

// Timers:
// 3 Mins to check wether Pressure is Stable or not
const unsigned long THREE_MIN = 180000; 

// Store time when pressure become low/stable
unsigned long lowStartTime = 0;
unsigned long stableStartTime = 0;

// Ensure msg sent only once. Otherwise msges send multiple times
bool lowMsgSent = false;
bool stableMsgSent = false;

// Alert Control
unsigned long alertStartTime = 0;
const unsigned long ALERT_DURATION = 3000;
const unsigned long ALERT_REPEAT_DELAY = 8000;
bool alertActive = false;

// OLED Control
bool oledFrozen = false;
unsigned long lastOledUpdate = 0;

// Blynk Timer
BlynkTimer timer;

// Functions:

void readDisplayAndSend() {

  // Read Temperature
  float temperature = bmp.readTemperature();
  // Skip the cycle to avoid wrong pressure calculation
  if (isnan(temperature) || temperature < -20 || temperature > 60) {
    Serial.println("BMP280 glitch detected  skipping cycle");
    return;
  }

  // Store Normal Pressure
  if (normal_temperature == 0) {
    normal_temperature = temperature;
  }

  // Calculating Pressure
  float pseudo_pressure =
    base_psi + (temperature - normal_temperature) * multiplier;

  // Low/Stable Logic
  if (LOWwarning) {
    if (pseudo_pressure >= warning_disappears) {
      LOWwarning = false;
      oledFrozen = false;
    }
  } else {
    if (pseudo_pressure <= warning_appear) {
      LOWwarning = true;
    }
  }

  // LED and Buzzer
  unsigned long now = millis();

  if (LOWwarning) {
    if (!alertActive && (now - alertStartTime >= ALERT_REPEAT_DELAY)) {
      alertActive = true;
      alertStartTime = now;
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(Buzzer_PIN, HIGH);
    }

    if (alertActive && (now - alertStartTime >= ALERT_DURATION)) {
      digitalWrite(LED_PIN, LOW);
      digitalWrite(Buzzer_PIN, LOW);
      alertActive = false;
    }
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(Buzzer_PIN, LOW);
    alertActive = false;
    alertStartTime = 0;
  }

  // OLED Update Controlls
  if (LOWwarning && millis() - lastOledUpdate < 4000) {
    return;   // reduce I2C stress during alert
  }
  lastOledUpdate = millis();

  if (!(LOWwarning && oledFrozen)) {

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("TPMS Monitor");
    display.println("----------------");

    display.print("Pressure: ");
    display.print(pseudo_pressure, 2);
    display.println(" PSI");

    display.print("Temp: ");
    display.print(temperature, 1);
    display.println(" C");

    if (LOWwarning) {
      display.setCursor(0, 48);
      display.println("WARNING: LOW PSI!");
      oledFrozen = true;
    }

    display.display();
  }

  // Blynk: Virtual Pin Assinging
  Blynk.virtualWrite(V0, pseudo_pressure);
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, LOWwarning ? 1 : 0);

  unsigned long currentTime = millis();

  // Notification Logic 

if (pseudo_pressure <= warning_appear) {
  // For Low Pressure
  stableStartTime = 0;
  stableMsgSent = false;

  if (lowStartTime == 0) {
    lowStartTime = currentTime;
  }

  if ((currentTime - lowStartTime >= THREE_MIN) && !lowMsgSent) {
    Blynk.logEvent("pressure_low", "Warning: Pressure is LOW!");
    lowMsgSent = true;
  }

} else {
  // For Stable Pressure
  lowStartTime = 0;
  lowMsgSent = false;

  if (stableStartTime == 0) {
    stableStartTime = currentTime;
  }

  if ((currentTime - stableStartTime >= THREE_MIN) && !stableMsgSent) {
    Blynk.logEvent("pressure_stable", "Pressure is stable");
    stableMsgSent = true;
  }
}

  //For Serial Monitor
  Serial.print("Pressure: ");
  Serial.print(pseudo_pressure);
  Serial.print(" PSI | Temp: ");
  Serial.print(temperature);
  Serial.println(" C");
}

// Setup

void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(Buzzer_PIN, OUTPUT);

  Wire.begin(21, 22);
  Wire.setClock(100000);   

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1);
  }

  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(0xFF);

  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 init failed");
    while (1);
  }

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, readDisplayAndSend);
}

//Loop
void loop() {
  Blynk.run();
  timer.run();
}