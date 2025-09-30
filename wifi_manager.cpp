#include "wifi_manager.h"
#include "config.h"
#include "oled_display.h"

void connectWiFi() {
  WiFi.begin(wifi_ssid, wifi_pass);
  Serial.print("SSID: ");
  Serial.println(wifi_ssid);
  Serial.print("Pass: ");
  Serial.println(wifi_pass);
  while (WiFi.status() != WL_CONNECTED) {
    displayMessage("WiFi", "Menghubungkan WiFi\n...");
  }
}

bool WiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool connectWiFiWithTimeout() {
    int attempts = 0;
    const int maxAttempts = 30; // Timeout setelah 15 detik (30 * 500ms)
    char spinner[] = "|/-\\";

    WiFi.begin(wifi_ssid, wifi_pass);
    
    Serial.println("===================================");
    Serial.print("Mencoba terhubung ke WiFi: ");
    Serial.println(wifi_ssid);
    Serial.print("Menghubungkan... "); 

    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        Serial.print(spinner[attempts % 4]);
        delay(500); 
        Serial.print("\b"); 
        
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("OK!"); 
        Serial.println("WiFi Terhubung!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.println("===================================");
        return true;
    } else {
        Serial.println("Gagal.");
        Serial.println("Gagal terhubung ke WiFi (Timeout).");
        Serial.println("===================================");
        WiFi.disconnect();
        return false;
    }
}
