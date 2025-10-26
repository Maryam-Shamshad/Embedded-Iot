// Name: Maryam Shamshad
// Reg No: 23-NTU-CS-1047
// Smart Button Control with OLED Feedback

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== OLED SETUP =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ===== PIN CONFIGURATION =====
#define LED_PIN    18      // LED output pin
#define BUZZER_PIN 27      // Buzzer output pin
#define BTN_PIN    33      // Push button input pin (with internal pull-up)

// ===== TIMING VARIABLES =====
unsigned long pressStart = 0;          // Stores the time when button is pressed
bool buttonPressed = false;            // Tracks if the button is currently pressed
bool ledState = false;                 // Current state of LED (ON/OFF)

const unsigned long LONG_PRESS_TIME = 1500;  // Long press threshold (1.5 seconds)


// ===== OLED DISPLAY FUNCTION =====
// Displays two lines of text on the OLED screen
void updateOLED(const String &line1, const String &line2 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println(line1);
  if (line2 != "") {
    display.setCursor(0, 30);
    display.println(line2);
  }
  display.display();
}


// ===== SHORT PRESS FUNCTION =====
// Toggles LED ON/OFF when button is briefly pressed
void handleShortPress() {
  ledState = !ledState;  // Toggle LED state
  digitalWrite(LED_PIN, ledState);
  Serial.println("Short Press → Toggle LED");
  updateOLED("Short Press", ledState ? "LED: ON" : "LED: OFF");
}


// ===== LONG PRESS FUNCTION =====
// Activates buzzer for a short tone when button is held
void handleLongPress() {
  Serial.println("Long Press → Play Buzzer");
  updateOLED("Long Press", "Buzzer Tone...");
  
  tone(BUZZER_PIN, 1000, 400);  // Generate 1 kHz tone for 400 ms
  delay(400);
  noTone(BUZZER_PIN);           // Stop buzzer tone
  
  updateOLED("Long Press", "Done");
}


// ===== SETUP FUNCTION =====
void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED initialization failed!");
    while (true); // Halt program if OLED is not found
  }

  display.clearDisplay();
  display.display();

  // Configure pin modes
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP); // Button connected to GND and input pin

  // Initial display message
  updateOLED("System Ready");
  Serial.println("System Ready");
}


// ===== MAIN LOOP =====
void loop() {
  int buttonState = digitalRead(BTN_PIN);

  // --- Button Press Detected (LOW due to INPUT_PULLUP) ---
  if (buttonState == LOW && !buttonPressed) {
    buttonPressed = true;
    pressStart = millis();  // Record the time of press
    updateOLED("Button Pressed...");
  }

  // --- Button Released ---
  if (buttonState == HIGH && buttonPressed) {
    buttonPressed = false;
    unsigned long pressDuration = millis() - pressStart; // Measure press duration

    // Decide action based on press time
    if (pressDuration > LONG_PRESS_TIME) {
      handleLongPress();   // Long press detected
    } else {
      handleShortPress();  // Short press detected
    }
  }

  delay(20); // Simple debounce delay to avoid false triggers
}
