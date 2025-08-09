#pragma once

#include <Arduino.h>
#include <MFRC522.h>

void initRFID();
String readCardUID();

extern MFRC522 mfrc522;