#include "api_task_manager.h"
#include "api_client.h"

static QueueHandle_t apiResultQueue;
static TaskHandle_t apiTaskHandle = NULL;

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


void setupApiTask() {
  apiResultQueue = xQueueCreate(1, sizeof(ApiResponse));

  if (apiResultQueue == NULL) {
    Serial.println("Kritis: Gagal membuat queue!");
    while(1);
  }
}

void startApiCheckTask(const String &uid) {
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
}

bool getApiResult(ApiResponse &result) {
  if (xQueueReceive(apiResultQueue, &result, (TickType_t)0) == pdPASS) {
    return true;
  }
  return false;
}

void stopApiCheckTask() {
  if (apiTaskHandle != NULL) {
    vTaskDelete(apiTaskHandle);
    Serial.println("Task API dihentikan paksa karena timeout.");
  }
}