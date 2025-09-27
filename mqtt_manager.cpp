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

void reconnectMQTT() {
  int attempts = 0;
  
  while (!client.connected()) {
    String loadingDots = "";
    int dotCount = (attempts % 3) + 1;
    for (int i = 0; i < dotCount; i++) {
      loadingDots += ".";
    }

    displayMessage("MQTT", "Menghubungkan\n" + loadingDots, 1);

    Serial.print("Menghubungkan ke MQTT ");
    Serial.print(loadingDots);

    // Coba koneksi MQTT
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_pass)) {
      Serial.println("\nTerhubung ke MQTT!");
      displayMessage("MQTT", "Terhubung!");
      client.subscribe("device/scan/request");
      client.subscribe("device/ping");
      delay(2500);
    } else {
      Serial.print("\nGagal, rc=");
      Serial.println(client.state());
      attempts++;
      delay(500);
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

  // Gabungkan jadi format JSON misalnya
  String payload = "{";
  payload += "\"uid\":\"" + mac + "\",";
  payload += "\"ip\":\"" + ip + "\"";
  payload += "}";

  client.publish("device/scan/response", payload.c_str());
}

