#include "mqtt_manager.h"
#include "config.h"
#include "buzzer.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

static WiFiClientSecure secureClient;
static PubSubClient client(secureClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("MQTT message arrived [");
  Serial.print(topic);
  Serial.print("] : ");
  Serial.println(message);

  if (String(topic) == "device/scan/request") {
    if (message == "start_scan") {
      publishSerialNumberDevice();
    }
  }
  if (String(topic) == "device/ping") {
    String deviceUid = getDeviceUid();
    if (message == deviceUid) {
      buzzerSuccess();
    }
  }
}

void setupMQTT() {
  secureClient.setInsecure(); // development only
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  reconnectMQTT();
}

void mqttLoop() {
  client.loop();
}

bool isMqttConnected() {
  return client.connected();
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    if (client.connect(mqttClientId, mqttUser, mqttPass)) {
      Serial.println("Terhubung ke MQTT!");
      client.subscribe("device/scan/request");
      client.subscribe("device/ping");
    } else {
      Serial.print("Gagal, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

static unsigned long lastMqttReconnectAttempt = 0;
void tryReconnectMQTT() {
  if (!client.connected()) {
    unsigned long now = millis();
    // Coba hubungkan ulang hanya setiap 5 detik
    if (now - lastMqttReconnectAttempt > 5000) {
      lastMqttReconnectAttempt = now;
      Serial.print("Mencoba menghubungkan ke MQTT...");
      if (client.connect(mqttClientId, mqttUser, mqttPass)) {
        Serial.println("Terhubung ke MQTT!");
        client.subscribe("device/scan/request");
        client.subscribe("device/ping");
      } else {
        Serial.print("Gagal, rc=");
        Serial.println(client.state());
      }
    }
  }
}

void publishMQTT(const String &uid) {
  String jsonPayload = "{\"uid\":\"" + uid + "\",\"device_uid\":\"" + getDeviceUid() + "\"}";
  client.publish(mqttTopicPublish, jsonPayload.c_str());
}

void publishSerialNumberDevice() {
  String mac = getDeviceUid();

  String ip = WiFi.localIP().toString();

  // Gabungkan jadi format JSON misalnya
  String payload = "{";
  payload += "\"uid\":\"" + mac + "\",";
  payload += "\"ip\":\"" + ip + "\"";
  payload += "}";

  client.publish("device/scan/response", payload.c_str());
}

