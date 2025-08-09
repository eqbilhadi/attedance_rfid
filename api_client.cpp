#include "api_client.h"
#include "config.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

ApiResponse checkRFID(const String &uid) {
  ApiResponse r;
  r.success = false;
  r.registered = false;
  r.message = "Unknown error";

  HTTPClient http;
  http.begin(apiEndpoint);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"uid\":\"" + uid + "\"}";
  int httpCode = http.POST(payload);
  String response = http.getString();

  if (httpCode > 0) {
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);
    if (!error) {
      r.success = true;
      r.registered = doc["data"]["registered"].as<bool>();
      r.message = doc["data"]["message"].as<String>();
    } else {
      r.message = "Gagal parsing response";
    }
  } else {
    r.message = "Request Gagal\nCode: " + String(httpCode) + "\nSegera hubungi admin";
  }

  http.end();
  return r;
}