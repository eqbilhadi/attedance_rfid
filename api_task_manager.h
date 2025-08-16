#pragma once

#include <Arduino.h>
#include "api_client.h"

void setupApiTask();
void startApiCheckTask(const String &uid);
bool getApiResult(ApiResponse &result);
void stopApiCheckTask();