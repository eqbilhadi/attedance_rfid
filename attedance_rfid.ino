#include "config.h"
#include "oled_display.h"
#include "buzzer.h"
#include "rfid_reader.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "api_client.h"

const int API_TIMEOUT_MS = 8000;
const int ANIMATION_DURATION_MS = 3000;
TaskHandle_t apiTaskHandle = NULL;
unsigned long checkStartTime = 0;
bool isCheckingRFID = false; 

QueueHandle_t apiResultQueue;

void apiCheckTask(void *parameter) {
  char* uidFromParam = (char*)parameter;
  String uid(uidFromParam);

  Serial.print("Memulai API task di background untuk UID: ");
  Serial.println(uid);
  
  ApiResponse result = checkRFID(uid);
  
  if (xQueueSend(apiResultQueue, &result, (TickType_t)0) != pdPASS) {
    Serial.println("Gagal mengirim hasil ke queue!");
  }

  Serial.println("API task selesai, hasil dikirim ke queue.");
  vTaskDelete(NULL); 
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  
  apiResultQueue = xQueueCreate(1, sizeof(ApiResponse));

  if (apiResultQueue == NULL) {
    Serial.println("Gagal membuat queue!");
    while(1);
  }

  initOLED();
  initBuzzer();
  initRFID();

  displayMessage("WiFi", "Menghubungkan...");
  connectWiFi();

  displayMessage("WAKTU", "Sinkronisasi...");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  int retry = 0;
  
  while (!getLocalTime(&timeinfo) || timeinfo.tm_year < 100) {
    Serial.println("Menunggu sinkronisasi waktu...");
    delay(500);
    retry++;
    if (retry > 20) { 
      Serial.println("Gagal mendapatkan waktu dari server NTP");
      displayMessage("ERROR", "Gagal Sinkronisasi Waktu");
      delay(3000);
      break; 
    }
  }

  if (retry <= 20) {
    Serial.println("Sinkronisasi waktu berhasil.");
  }

  displayMessage("WiFi Terhubung", WiFi.localIP().toString());
  delay(2000);
  
  setupMQTT();
}

void loop() {
  if (isCheckingRFID) {
    ApiResponse receivedResult;
    
    if (xQueueReceive(apiResultQueue, &receivedResult, (TickType_t)0) == pdPASS) {
      isCheckingRFID = false; 

      drawWaitingAnimation("MEMERIKSA", 100);
      delay(200);

      if (receivedResult.success) {
        buzzerSuccess();
        displayMessage(receivedResult.title, receivedResult.message);
        if (!receivedResult.registered) {
          publishMQTT(receivedResult.uid); 
        }
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
      vTaskDelete(apiTaskHandle);
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
    if (!isMqttConnected()) reconnectMQTT();
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
        
        xTaskCreate(
          apiCheckTask,
          "APICheckTask",
          8192,            
          (void*)uidBuffer, 
          1,
          &apiTaskHandle
        );
      } else {
        buzzerError();
        displayMessage("WiFi Putus", "Periksa koneksi!");
        delay(2500);
        handleIdleScreen();
      }
    }
  }
}