#include "wifi_manager.h"
#include "config.h"
#include "oled_display.h"

void connectWiFi() {
  WiFi.begin(wifi_ssid, wifi_pass);
  Serial.print("SSID: ");
  Serial.println(wifi_ssid);
  Serial.print("Pass: "); // Hapus baris ini setelah debug untuk keamanan
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
  const int maxAttempts = 30;
  char spinner[] = "|/-\\";

  WiFi.begin(wifi_ssid, wifi_pass);
  
  Serial.println("===================================");
  Serial.print("Mencoba terhubung ke WiFi: ");
  Serial.println(wifi_ssid);
  Serial.print("Menghubungkan... "); 

  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    String loadingDots = "";
    int dotCount = (attempts % 3) + 1;
    for (int i = 0; i < dotCount; i++) {
      loadingDots += ".";
    }
    
    displayMessage("WiFi", "Menghubungkan\n" + loadingDots, 1);

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
    
    displayMessage("WiFi Terhubung!", "IP: " + WiFi.localIP().toString() + "\nSSID: " + wifi_ssid, 1);
    delay(2500);
    return true; 
  } else {
    Serial.println("Gagal.");
    Serial.println("Gagal terhubung ke WiFi (Timeout).");
    Serial.println("===================================");
    WiFi.disconnect();

    // Tampilkan pesan gagal final di OLED
    displayMessage("Koneksi Gagal", "Cek SSID/Pass WiFi", 1);
    delay(2500);
    return false;
  }
}
