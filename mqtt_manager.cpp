#include "mqtt_manager.h"
#include "config.h"
#include "buzzer.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "oled_display.h"

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
  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(callback);
  reconnectMQTT();
}

void mqttLoop() {
  client.loop();
}

bool isMqttConnected() {
  return client.connected();
}

bool reconnectMQTT() {
    secureClient.setInsecure();
    client.setServer(mqtt_server, atoi(mqtt_port));
    client.setCallback(callback);

    Serial.println("===================================");
    Serial.print("Mencoba terhubung ke MQTT Broker...");

    int attempts = 0;
    const int maxAttempts = 5;

    while (!client.connected() && attempts < maxAttempts) {
        Serial.print("\nPercobaan ke-");
        Serial.print(attempts + 1);
        Serial.print("... ");

        if (client.connect(mqtt_client_id, mqtt_user, mqtt_pass)) {
            Serial.println("OK!");
            Serial.println("Terhubung ke MQTT Broker.");
            
            client.subscribe("device/scan/request");
            client.subscribe("device/ping");
            
            Serial.println("===================================");
            return true;
        } else {
            Serial.print("Gagal, status client = ");
            Serial.print(client.state());
            delay(2000); 
        }
        attempts++;
    }

    Serial.println("\nGagal terhubung ke MQTT Broker setelah beberapa kali percobaan.");
    Serial.println("===================================");
    return false;
}


static unsigned long lastMqttReconnectAttempt = 0;
void tryReconnectMQTT() {
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastMqttReconnectAttempt > 5000) {
      lastMqttReconnectAttempt = now;
      Serial.print("Mencoba menghubungkan ke MQTT...");
      if (client.connect(mqtt_client_id, mqtt_user, mqtt_pass)) {
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
  client.publish("alat/rfid", jsonPayload.c_str());
}

void publishSerialNumberDevice() {
  String mac = getDeviceUid();

  String ip = WiFi.localIP().toString();

  String payload = "{";
  payload += "\"uid\":\"" + mac + "\",";
  payload += "\"ip\":\"" + ip + "\"";
  payload += "}";

  client.publish("device/scan/response", payload.c_str());
}

