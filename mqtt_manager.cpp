#include "mqtt_manager.h"
#include "config.h"
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
    } else {
      Serial.print("Gagal, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void publishMQTT(const String &uid) {
  String jsonPayload = "{\"uid\":\"" + uid + "\"}";
  client.publish(mqttTopicPublish, jsonPayload.c_str());
}

void publishSerialNumberDevice() {
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  mac.toLowerCase();
  client.publish("device/scan/response", mac.c_str());
}
