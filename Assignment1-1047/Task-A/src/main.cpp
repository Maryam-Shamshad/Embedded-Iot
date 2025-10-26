//Name: Maryam Shamshad
//Reg no: 23-NTU-CS-1047
//Project: Multi-Mode LEDs with OLED Display

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== OLED CONFIGURATION =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ===== PIN CONFIGURATION =====
#define LED_A 16             // First LED
#define LED_B 17             // Second LED
#define LED_FADE 18          // PWM LED for fading effect
#define BUTTON_MODE 33       // Button to switch modes
#define BUTTON_RESET 27     // Button to reset to Mode 0

// ===== VARIABLES =====
volatile bool modeButtonPressed = false;
volatile bool resetButtonPressed = false;

uint8_t currentMode = 0;                     // Current operating mode (0–3)
unsigned long lastDebounceTime = 0;          // Last debounce timestamp
const unsigned long debounceDelay = 200;     // Debounce delay (milliseconds)

unsigned long previousBlinkTime = 0;         // Timer for blinking control
const unsigned long blinkInterval = 500;     // LED blink delay (milliseconds)

// PWM fading variables
int fadeValue = 0;                           // Current brightness (0–255)
int fadeDirection = 1;                       // Direction of fade (1 = up, -1 = down)


// ===== INTERRUPT HANDLERS =====
void handleModeButton() {
  unsigned long now = millis();
  if (now - lastDebounceTime > debounceDelay) {
    modeButtonPressed = true;
    lastDebounceTime = now;
  }
}

void handleResetButton() {
  unsigned long now = millis();
  if (now - lastDebounceTime > debounceDelay) {
    resetButtonPressed = true;
    lastDebounceTime = now;
  }
}


// ===== DISPLAY HELPER FUNCTION =====
void showOnOLED(const String &line1, const String &line2 = "") {
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


// ===== MODE FUNCTION =====
void setOperatingMode(uint8_t newMode) {
  currentMode = newMode;
  analogWrite(LED_FADE, 0); // Stop any active PWM before switching modes

  switch (currentMode) {
    case 0: // OFF mode
      digitalWrite(LED_A, LOW);
      digitalWrite(LED_B, LOW);
      showOnOLED("Mode 0: Both OFF");
      break;

    case 1: // Alternate blinking mode
      showOnOLED("Mode 1: Alternate Blink");
      break;

    case 2: // Both LEDs ON
      digitalWrite(LED_A, HIGH);
      digitalWrite(LED_B, HIGH);
      showOnOLED("Mode 2: Both ON");
      break;

    case 3: // PWM fading mode
      showOnOLED("Mode 3: LED3 PWM Fade");
      break;
  }
}


// ===== SETUP FUNCTION =====
void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED initialization failed!");
    while (true); // Halt if display not found
  }
  display.clearDisplay();
  display.display();

  // Set pin modes
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_FADE, OUTPUT);
  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(BUTTON_RESET, INPUT_PULLUP);

  // Attach button interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_MODE), handleModeButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RESET), handleResetButton, FALLING);

  // Initialize system
  setOperatingMode(0);
  showOnOLED("System Ready", "Press Mode Button");
}


// ===== MAIN LOOP =====
void loop() {
  // --- Handle button actions ---
  if (modeButtonPressed) {
    modeButtonPressed = false;
    currentMode = (currentMode + 1) % 4; // Cycle through 0–3
    setOperatingMode(currentMode);
  }

  if (resetButtonPressed) {
    resetButtonPressed = false;
    setOperatingMode(0); // Return to Mode 0

    // ===== OLED Reset Message =====
    showOnOLED("System Reset", "Back to Mode 0");

    // ===== Visual Confirmation (optional) =====
    digitalWrite(LED_A, HIGH);
    digitalWrite(LED_B, HIGH);
    delay(300);
    digitalWrite(LED_A, LOW);
    digitalWrite(LED_B, LOW);

    delay(1000); // Show reset message for 1 second
    showOnOLED("Mode 0: Both OFF"); // Restore mode display
  }

  // --- Execute current mode behavior ---
  if (currentMode == 1) { // Alternate blink
    if (millis() - previousBlinkTime >= blinkInterval) {
      previousBlinkTime = millis();
      digitalWrite(LED_A, !digitalRead(LED_A));
      digitalWrite(LED_B, !digitalRead(LED_B));
    }
  } 
  else if (currentMode == 3) { // PWM fade effect
    fadeValue += fadeDirection * 5;
    if (fadeValue >= 255 || fadeValue <= 0) fadeDirection *= -1;
    analogWrite(LED_FADE, fadeValue);
    delay(20); // Smooth fading
  }
}
