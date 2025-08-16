#pragma once
#include <Arduino.h>

void setupMQTT();
void mqttLoop();
void reconnectMQTT();
void tryReconnectMQTT();
bool isMqttConnected();
void publishMQTT(const String &uid);
void publishSerialNumberDevice();