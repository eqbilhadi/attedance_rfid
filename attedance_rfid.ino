#include "config.h"
#include "oled_display.h"
#include "buzzer.h"
#include "rfid_reader.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "api_client.h"
#include "api_task_manager.h"
#include "access_points.h"

const int API_TIMEOUT_MS = 8000;
const int ANIMATION_DURATION_MS = 4000;
unsigned long checkStartTime = 0;
bool isCheckingRFID = false; 

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  setupApiTask();
  initOLED();
  initBuzzer();
  initRFID();

  displayMessage("SISTEM PRESENSI", "Memuat...", 1);
  delay(2000);

  StatusType wifiStatus = STATUS_PENDING;
  StatusType timeStatus = STATUS_PENDING;
  StatusType mqttStatus = STATUS_PENDING;

  drawStatusScreen(wifiStatus, timeStatus, mqttStatus);
  delay(500);

  if (loadConfig()) {
    if (connectWiFiWithTimeout()) {
      wifiStatus = STATUS_SUCCESS;
    } else {
      wifiStatus = STATUS_FAILED;
    }
  } else {
    wifiStatus = STATUS_FAILED;
  }
  drawStatusScreen(wifiStatus, timeStatus, mqttStatus);
  delay(1000);

  if (wifiStatus == STATUS_FAILED) {
    displayMessage("WIFI GAGAL", "Masuk ke mode AP");
    startAPMode();
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  int retry = 0;
  while (!getLocalTime(&timeinfo, 5000) || timeinfo.tm_year < 100) {
    retry++;
    if (retry > 10) {
      break; 
    }
    delay(500);
  }

  if (retry <= 10) {
    timeStatus = STATUS_SUCCESS;
    Serial.println("Waktu berhasil disinkronkan");
  } else {
    timeStatus = STATUS_FAILED;
    Serial.println("Gagal sinkronisasi waktu");
  }
  drawStatusScreen(wifiStatus, timeStatus, mqttStatus);
  delay(1000);

  if (reconnectMQTT()) {
    mqttStatus = STATUS_SUCCESS;
  } else {
    mqttStatus = STATUS_FAILED;
  }
  drawStatusScreen(wifiStatus, timeStatus, mqttStatus);
  delay(2500); 

  Serial.println("Setup Selesai. Masuk ke loop utama.");
  display.clearDisplay();
  display.display();
}

void loop() {
  if (isCheckingRFID) {
    ApiResponse receivedResult;
    
    if (getApiResult(receivedResult)) {
      isCheckingRFID = false; 

      drawWaitingAnimation("MEMERIKSA", 100);
      delay(200);

      if (receivedResult.success) {
        if (!receivedResult.registered) {
          publishMQTT(receivedResult.uid); 
        }
        buzzerSuccess();
        displayMessage(receivedResult.title, receivedResult.message);
        onLedGreen();
      } else {
        buzzerError();
        displayMessage("ERROR", receivedResult.message);
      }
      delay(3000);
      handleIdleScreen();
      return;
    }
    
    unsigned long elapsedTime = millis() - checkStartTime;
    if (elapsedTime > API_TIMEOUT_MS) {
      Serial.println("API Timeout!");
      stopApiCheckTask();
      isCheckingRFID = false;

      buzzerError();
      displayMessage("TIMEOUT", "Server tidak merespon");
      delay(3000);
      handleIdleScreen();
      return;
    }
    
    int progress = map(elapsedTime, 0, ANIMATION_DURATION_MS, 0, 100); 
    progress = constrain(progress, 0, 100); 
    drawWaitingAnimation("MEMERIKSA", progress);
    delay(50);
  } 
  else {
    if (!isMqttConnected()) tryReconnectMQTT();
    mqttLoop();
    handleIdleScreen();
    String uid = readCardUID();
    if (uid != "") {
      Serial.println("UID Terdeteksi: " + uid);
      digitalWrite(BUZZER_PIN, HIGH); delay(100); digitalWrite(BUZZER_PIN, LOW);
      
      if (WiFi.status() == WL_CONNECTED) {
        checkStartTime = millis();
        isCheckingRFID = true; 
        
        static char uidBuffer[33]; 
        uid.toCharArray(uidBuffer, sizeof(uidBuffer));
        
        startApiCheckTask(uid);
      } else {
        buzzerError();
        displayMessage("WiFi Putus", "Periksa koneksi!");
        delay(2500);
        handleIdleScreen();
      }
    }
  }
}
