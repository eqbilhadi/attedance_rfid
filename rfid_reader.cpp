#include "rfid_reader.h"
#include "config.h"
#include <SPI.h>

MFRC522 mfrc522(SS_PIN, RST_PIN);

void initRFID() {
  SPI.begin();
  mfrc522.PCD_Init();
}

String readCardUID() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return "";

  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uidStr += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidStr.toUpperCase();
  return uidStr;
}