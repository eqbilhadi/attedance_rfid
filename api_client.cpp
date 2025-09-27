#include "api_client.h"
#include "config.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

ApiResponse checkRFID(const String &uid) {
  ApiResponse r;
  r.uid = uid;
  r.success = false;
  r.registered = false;
  r.message = "Unknown error";
  r.title = "ERROR";

  HTTPClient http;
  http.begin(api_url);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"uid\":\"" + uid + "\",\"device_uid\":\"" + getDeviceUid() + "\"}";
  
  Serial.println("[HTTP] Sending request: " + payload);

  http.setTimeout(10000);  // 10 detik
  int httpCode = http.POST(payload);
  String response = http.getString();

  Serial.print("[HTTP] Response code: ");
  Serial.println(httpCode);
  Serial.println("[HTTP] Raw response: " + response);

  if (httpCode >= 200 && httpCode < 300) {
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);
    if (!error) {
      r.success = true;
      r.registered = doc["data"]["registered"] | false;
      r.message = doc["data"]["message"] | "No message";
      r.title = doc["data"]["title"] | "No title";

      Serial.println("[JSON] Parse OK");
      Serial.println("[JSON] registered: " + String(r.registered));
      Serial.println("[JSON] message: " + r.message);
      Serial.println("[JSON] title: " + r.title);
    } else {
      r.message = "Gagal parsing response";
      Serial.println("[JSON] Parse error: " + String(error.c_str()));
    }
  } else {
    r.message = "Request Gagal\nCode: " + String(httpCode) + "\nSegera hubungi admin";
    Serial.println("[HTTP] Error: " + r.message);  
    Serial.printf("[HTTP] Error detail: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  return r;
}
