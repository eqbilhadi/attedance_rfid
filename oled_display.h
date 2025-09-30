#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <vector>
#include <string>

enum StatusType {
  STATUS_PENDING,
  STATUS_SUCCESS,
  STATUS_FAILED
};

extern const unsigned char PROGMEM icon_checkmark_bits[];
extern const unsigned char PROGMEM icon_cross_bits[];
extern const unsigned char PROGMEM image_clock_quarters_bits[];
extern const unsigned char PROGMEM image_network_www_bits[];
extern const unsigned char PROGMEM image_wifi_full_bits[];

void initOLED();
void displayMessage(String title = "TPQ DARUL QURRO", String msg = "", int textSize = 1);
void showLoading(String message);
void updateDisplayMode(const String& dateStr, const String& timeStr);
void showCenteredText(String text, int y, int textSize = 1);
void wrapCenteredText(String text, int startY, int textSize = 1);
void showProgressBar(String labelText, int durationMillis);
void drawWaitingAnimation(const String& label, int progress);
void handleIdleScreen(); 
void drawStatusLine(int y, const unsigned char* icon_bits, int icon_w, int icon_h, String text, StatusType status);
void drawStatusScreen(StatusType wifiStatus, StatusType timeStatus, StatusType mqttStatus);

extern TwoWire myWire;
extern Adafruit_SSD1306 display;