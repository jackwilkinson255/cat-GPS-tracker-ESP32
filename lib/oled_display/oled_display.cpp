#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "oled_display.h"

// #define BUTTON_A 15
// #define BUTTON_B 32
// #define BUTTON_C 14
#define WIRE Wire1

// For the STEMMA QT cables, we follow the Qwiic convention:

// Black for GND
// Red for V+
// Blue for SDA
// Yellow for SCL


Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &WIRE);


void OLEDDisplay::setup(void)
{
  Wire1.begin(41, 40);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  Serial.println("OLED begun");

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}


void OLEDDisplay::updateDisplay(char *timestamp, char *location, bool fix, char *battery, char *wifi_details)
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(timestamp);
  display.println(location);
  display.println(wifi_details);
  display.println(battery);
  display.display();
}