#pragma once

#include <Arduino.h>

struct ApiResponse {
  bool success;
  bool registered;
  String message;
  String title;
};

ApiResponse checkRFID(const String &uid);