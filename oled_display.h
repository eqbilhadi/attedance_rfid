#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <vector>
#include <string>

void initOLED();
void displayMessage(String title = "TPQ DARUL QURRO", String msg = "", int textSize = 1);
void showLoading(String message);
void updateDisplayMode(const String& dateStr, const String& timeStr);
void showCenteredText(String text, int y, int textSize = 1);
void wrapCenteredText(String text, int startY, int textSize = 1);
void showProgressBar(String labelText, int durationMillis);
void drawWaitingAnimation(const String& label, int progress);
void handleIdleScreen(); 

extern TwoWire myWire;
extern Adafruit_SSD1306 display;