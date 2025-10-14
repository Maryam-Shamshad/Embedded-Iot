#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---- OLED setup ----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Wire.begin(21, 22); // ESP32 default I2C pins (SDA=21, SCL=22)

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    for (;;); // Stop if OLED not found
  }

  display.clearDisplay();

  // ---- Display Name and ID ----
  display.setTextSize(1);                // Small text
  display.setTextColor(SSD1306_WHITE);   // White text
  display.setCursor(1, 22);             // Slightly lower Y for centering
  display.println("Name: Maryam Shamshad");
  display.setCursor(1, 38);
  display.println("ID: 23-NTU-CS-1047");

  // ---- Adjusted rectangle (perfect fit) ----
  display.drawRect(0, 15, 128, 40, SSD1306_WHITE);
  // (x=0, y=15, width=128, height=40)

  display.display(); // Show everything on OLED
}

void loop() {
  // Nothing here — static display
}
