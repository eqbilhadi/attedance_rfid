#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// WiFi credentials
const char* ssid = "Billy Aufa_EXT";
const char* password = "Samawa7371";

// RFID
#define SS_PIN 21
#define RST_PIN 22

// Buzzer
#define BUZZER_PIN 15

// OLED Display
#define OLED_SDA 4 
#define OLED_SCL 5
#define OLED_RESET -1

TwoWire myWire = TwoWire(1);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &myWire, OLED_RESET);

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);

  // Inisialisasi SPI dan RFID
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.println("Memulai sistem...");

  // Inisialisasi I2C untuk OLED
  myWire.begin(OLED_SDA, OLED_SCL);
  delay(100);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Gagal inisialisasi OLED!"));
    while (true);
  }

  delay(100);
  display.clearDisplay();
  display.display();

  // Tampilkan pesan koneksi WiFi
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Menghubungkan ke WiFi");
  display.display();

  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Simulasi loading titik-titik
  int dotX = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.setCursor(dotX, 10);
    display.print(".");
    display.display();
    dotX += 6;
    if (dotX > 120) {
      dotX = 0;
      display.fillRect(0, 10, 128, 10, SSD1306_BLACK);
    }
  }

  // Jika tetap belum connect setelah 10x, lanjut aja biar ga macet
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Gagal konek WiFi.");
    display.setCursor(0, 24);
    display.println("Gagal konek WiFi.");
    display.display();
    delay(3000);
  } else {
    Serial.println("");
    Serial.println("WiFi terhubung.");
    Serial.println(WiFi.localIP());

    // Tampilkan info koneksi
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi Terhubung!");
    display.setCursor(0, 12);
    display.println("SSID:");
    display.setCursor(0, 22);
    display.println(ssid);
    display.setCursor(0, 36);
    display.println("IP:");
    display.setCursor(0, 46);
    display.println(WiFi.localIP());
    display.display();
    delay(3000);
  }

  // Tampilkan layar awal absensi
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(28, 0);
  display.println("ABSENSI RFID");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  display.setCursor(10, 30);
  display.println("Tempelkan kartu");
  display.display();
}


void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  // Baca UID dan tampilkan
  String uidStr = "";
  Serial.print("UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(mfrc522.uid.uidByte[i], HEX);

    uidStr += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // 🔊 Bunyikan buzzer langsung setelah UID kebaca
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);

  // 📺 Tampilkan UID ke OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(28, 0);
  display.println("ABSENSI RFID");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  display.setCursor(0, 18);
  display.println("UID:");

  display.setTextSize(2);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(uidStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 32);
  display.println(uidStr);
  display.display();

  delay(2000); // Tunggu 2 detik

  // ↩️ Kembali ke tampilan awal
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(28, 0);
  display.println("ABSENSI RFID");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  display.setCursor(10, 30);
  display.println("Tempelkan kartu");
  display.display();

  delay(1000); // Hindari double scan
}


