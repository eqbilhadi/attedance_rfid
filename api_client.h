#pragma once

#include <Arduino.h>

struct ApiResponse {
  bool success;
  bool registered;
  String message;
  String title;
  String uid;
};

ApiResponse checkRFID(const String &uid);