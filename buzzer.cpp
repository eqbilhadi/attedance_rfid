#include "buzzer.h"
#include "config.h"
#include <Arduino.h>

void initBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void buzzerSuccess() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}

void buzzerError() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
}

void onLedGreen() {
  digitalWrite(LED_GREEN, HIGH);  // LED ON
  delay(1000);
  digitalWrite(LED_GREEN, LOW);  // LED OFF
  delay(1000);
}