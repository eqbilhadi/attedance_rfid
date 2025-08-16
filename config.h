#pragma once

#include <WiFi.h>

// === WiFi Settings ===
static const char* ssid = "Billy Aufa_EXT";
static const char* password = "Samawa7371";

// === MQTT Settings ===
static const char* mqttServer = "6f7e4be549454cf493675cf1b528d102.s1.eu.hivemq.cloud";
static const int mqttPort = 8883;
static const char* mqttUser = "esp32_user";
static const char* mqttPass = "Esp32_password";
static const char* mqttClientId = "alat-rfid-01";
static const char* mqttTopicPublish = "alat/rfid";

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

// API Endpoint
static const char* apiEndpoint = "http://192.168.1.8:8000/api/check-rfid";

// Get time config
static const char* ntpServer = "pool.ntp.org";
static const long gmtOffset_sec = 7 * 3600; // WIB adalah UTC+7
static const int daylightOffset_sec = 0;  

// Device UID or Serial number of device
inline String getDeviceUid() {
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  mac.toLowerCase();

  return mac;
}