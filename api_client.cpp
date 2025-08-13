#include "api_client.h"
#include "config.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

ApiResponse checkRFID(const String &uid) {
  ApiResponse r;
  r.success = false;
  r.registered = false;
  r.message = "Unknown error";
  r.title = "ERROR";

  HTTPClient http;
  http.begin(apiEndpoint);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"uid\":\"" + uid + "\",\"device_uid\":\"" + getDeviceUid() + "\"}";
  int httpCode = http.POST(payload);
  String response = http.getString();

  if (httpCode >= 200 && httpCode < 300) {
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);
    if (!error) {
      r.success = true;
      r.registered = doc["data"]["registered"] | false;
      r.message = doc["data"]["message"] | "No message";
      r.title = doc["data"]["title"] | "No title";
    } else {
      r.message = "Gagal parsing response";
    }
  } else {
    r.message = "Request Gagal\nCode: " + String(httpCode) + "\nSegera hubungi admin";
  }

  http.end();
  return r;
}
