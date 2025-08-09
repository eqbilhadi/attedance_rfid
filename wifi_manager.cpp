#include "wifi_manager.h"
#include "config.h"
#include <WiFi.h>

void connectWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

bool WiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}