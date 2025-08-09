#pragma once

#include <Arduino.h>

struct ApiResponse {
  bool success;
  bool registered;
  String message;
};

ApiResponse checkRFID(const String &uid);