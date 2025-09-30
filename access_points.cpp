#include <WebServer.h>
#include <DNSServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "config.h"
#include "oled_display.h"

DNSServer dnsServer;
WebServer webServer(80);
const char* ap_ssid = "ESP32-Konfigurasi";

bool loadConfig() {
  if (!SPIFFS.begin(true)) {
    Serial.println("Gagal me-mount SPIFFS");
    return false;
  }

  if (SPIFFS.exists("/config.json")) {
    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile) {
      Serial.println("Gagal membuka config");
      return false;
    }

    Serial.println("=== Isi /config.json ===");
    while (configFile.available()) {
      Serial.write(configFile.read());
    }
    Serial.println("\n========================");
    
    configFile.seek(0, SeekSet);

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();

    if (error) {
      Serial.print("Gagal parsing JSON: ");
      Serial.println(error.f_str());
      return false;
    }

    strlcpy(wifi_ssid, doc["wifi_ssid"] | "", sizeof(wifi_ssid));
    strlcpy(wifi_pass, doc["wifi_pass"] | "", sizeof(wifi_pass));
    strlcpy(mqtt_server, doc["mqtt_server"] | "", sizeof(mqtt_server));
    strlcpy(mqtt_port, doc["mqtt_port"] | "", sizeof(mqtt_port));
    strlcpy(mqtt_user, doc["mqtt_user"] | "", sizeof(mqtt_user));
    strlcpy(mqtt_pass, doc["mqtt_pass"] | "", sizeof(mqtt_pass));
    strlcpy(mqtt_client_id, doc["mqtt_client_id"] | "", sizeof(mqtt_client_id));
    strlcpy(api_url, doc["api_url"] | "", sizeof(api_url));

    Serial.println("=== Konfigurasi Dimuat dari /config.json ===");
    Serial.print("WiFi SSID     : "); Serial.println(wifi_ssid);
    Serial.print("WiFi Pass     : "); Serial.println(wifi_pass);
    Serial.print("MQTT Server   : "); Serial.println(mqtt_server);
    Serial.print("MQTT Port     : "); Serial.println(mqtt_port);
    Serial.print("MQTT User     : "); Serial.println(mqtt_user);
    Serial.print("MQTT Pass     : "); Serial.println(mqtt_pass);
    Serial.print("MQTT ClientID : "); Serial.println(mqtt_client_id);
    Serial.print("API URL       : "); Serial.println(api_url);
    Serial.println("===========================================");

    return true;
  } else {
    Serial.println("Config file tidak ditemukan.");
    return false;
  }
}


bool saveConfig() {
  StaticJsonDocument<512> doc;

  String ssid = webServer.arg("ssid");
  String pass = webServer.arg("pass");
  String mqttServ = webServer.arg("mqtt_server");
  String mqttPrt = webServer.arg("mqtt_port");
  String mqttUsr = webServer.arg("mqtt_user");
  String mqttPsw = webServer.arg("mqtt_pass");
  String mqttClient = webServer.arg("mqtt_client_id");
  String apiUrl = webServer.arg("api_url");

  Serial.println("=== Data Konfigurasi Diterima ===");
  Serial.println("WiFi SSID     : " + ssid);
  Serial.println("WiFi Pass     : " + pass);
  Serial.println("MQTT Server   : " + mqttServ);
  Serial.println("MQTT Port     : " + mqttPrt);
  Serial.println("MQTT User     : " + mqttUsr);
  Serial.println("MQTT Pass     : " + mqttPsw);
  Serial.println("MQTT ClientID : " + mqttClient);
  Serial.println("API URL       : " + apiUrl);
  Serial.println("================================");

  doc["wifi_ssid"] = ssid;
  doc["wifi_pass"] = pass;
  doc["mqtt_server"] = mqttServ;
  doc["mqtt_port"] = mqttPrt;
  doc["mqtt_user"] = mqttUsr;
  doc["mqtt_pass"] = mqttPsw;
  doc["mqtt_client_id"] = mqttClient;
  doc["api_url"] = apiUrl;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Gagal membuat config");
    return false;
  }

  serializeJson(doc, configFile);
  configFile.close();

  Serial.println("Config berhasil disimpan ke /config.json");

  return true;
}

void startAPMode() {
  display.clearDisplay();
  display.setCursor(0, 0);
  Serial.println("Mode Konfigurasi");

  WiFi.softAP(ap_ssid);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("AP IP: ");
  Serial.println(apIP.toString());

  displayMessage("MODE KONFIGURASI", "IP: " + apIP.toString() + "\nHubungkan ke WiFi: " + ap_ssid);

  dnsServer.start(53, "*", apIP);

  webServer.on("/", HTTP_GET, []() {
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
      webServer.send(404, "text/plain", "File index.html tidak ditemukan");
      return;
    }

    String page = file.readString();
    file.close();

    page.replace("%WIFI_SSID%", String(wifi_ssid));
    page.replace("%WIFI_PASS%", String(wifi_pass));
    page.replace("%MQTT_SERVER%", String(mqtt_server));
    page.replace("%MQTT_PORT%", String(mqtt_port));
    page.replace("%MQTT_USER%", String(mqtt_user));
    page.replace("%MQTT_PASS%", String(mqtt_pass));
    page.replace("%MQTT_CLIENT_ID%", String(mqtt_client_id));
    page.replace("%API_URL%", String(api_url));

    webServer.send(200, "text/html", page);
  });

  webServer.on("/save", HTTP_GET, []() {
    display.clearDisplay();
    display.setCursor(0,0);
    Serial.println("Menyimpan config...");
    webServer.send(200, "text/html", "<h1>Konfigurasi Disimpan!</h1><p>Perangkat akan me-restart dalam 5 detik...</p>");
    delay(1000);
    if (saveConfig()) {
      Serial.println("Config berhasil disimpan.");
      Serial.println("Restarting...");
      delay(4000);
      ESP.restart();
    } else {
      Serial.println("Gagal menyimpan!");
    }
  });

  webServer.onNotFound([]() {
    webServer.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
    webServer.send(302, "text/plain", "");
  });

  webServer.begin();
  Serial.println("Hubungkan ke WiFi");
  Serial.println(ap_ssid);

  while (true) {
    dnsServer.processNextRequest();
    webServer.handleClient();
    delay(10);
  }
}