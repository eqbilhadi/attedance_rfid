#pragma once

#include <WiFi.h>

// === WiFi Settings (bisa diubah runtime) ===
extern char wifi_ssid[32];
extern char wifi_pass[64];

// === MQTT Settings (bisa diubah runtime) ===
extern char mqtt_server[100];
extern char mqtt_port[6];
extern char mqtt_user[32];
extern char mqtt_pass[32];
extern char mqtt_client_id[32];

// API Endpoint (bisa diubah runtime)
extern char api_url[128];

// Get time config
static const char* ntpServer = "pool.ntp.org";
static const long gmtOffset_sec = 7 * 3600; // WIB adalah UTC+7
static const int daylightOffset_sec = 0;  

// === OLED ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 4
#define OLED_SCL 5
#define OLED_RESET -1

// === RFID ===
#define SS_PIN 21
#define RST_PIN 22

// === Buzzer ===
#define BUZZER_PIN 2

// === LED_LAMP ===
#define LED_GREEN 15

// Device UID or Serial number of device
inline String getDeviceUid() {
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  mac.toLowerCase();

  return mac;
}