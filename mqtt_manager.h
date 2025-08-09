#pragma once
#include <Arduino.h>

void setupMQTT();
void mqttLoop();
void reconnectMQTT();
bool isMqttConnected();
void publishMQTT(const String &uid);
void publishSerialNumberDevice();