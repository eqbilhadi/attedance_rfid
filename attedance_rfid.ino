#include "config.h"
#include "oled_display.h"
#include "buzzer.h"
#include "rfid_reader.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "api_client.h"

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);

  initOLED();
  initBuzzer();
  initRFID();

  displayMessage("WiFi", "Menghubungkan...");
  connectWiFi();

  displayMessage("WiFi Terhubung", WiFi.localIP().toString());
  delay(2000);

  // MQTT
  setupMQTT();

  updateDisplayMode();
}

void loop() {
  if (!isMqttConnected()) reconnectMQTT();
  mqttLoop();

  String uid = readCardUID();
  if (uid == "") return;

  Serial.println("UID: " + uid);

  // short beep
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);

  if (WiFi.status() == WL_CONNECTED) {
    showProgressBar("Memeriksa kartu", 500);
    auto res = checkRFID(uid);

    if (res.success) {
      if (res.registered) {
        displayMessage(res.title, res.message);
      } else {
        displayMessage(res.title, res.message);
        // publish to MQTT when not registered
        publishMQTT(uid);
      }
      buzzerSuccess();
    } else {
      buzzerError();
      displayMessage("ERROR", res.message);
    }
  } else {
    buzzerError();
    displayMessage("WiFi Putus", "Periksa koneksi!");
  }

  delay(2500);
  updateDisplayMode();
}