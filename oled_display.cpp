#include "oled_display.h"
#include "config.h"
#include <time.h>

TwoWire myWire = TwoWire(1);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &myWire, OLED_RESET);

void splitWordsIntoLines(String paragraph, std::vector<String>& lines, int charWidth, int maxWidth) {
  std::vector<String> words;
  String currentWord = "";

  for (int i = 0; i < paragraph.length(); i++) {
    if (paragraph[i] == ' ') {
      if (currentWord != "") words.push_back(currentWord);
      currentWord = "";
    } else {
      currentWord += paragraph[i];
    }
  }
  if (currentWord != "") words.push_back(currentWord);

  String line = "";
  for (int i = 0; i < words.size(); i++) {
    String testLine = line.length() > 0 ? line + " " + words[i] : words[i];
    if (testLine.length() * charWidth <= maxWidth) {
      line = testLine;
    } else {
      if (line != "") lines.push_back(line);
      line = words[i];
    }
  }
  if (line != "") lines.push_back(line);
}

void initOLED() {
  myWire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Gagal inisialisasi OLED!");
    while (true);
  }
  display.clearDisplay();
}

void showCenteredText(String text, int y, int textSize) {
  display.setTextSize(textSize);
  int16_t x = (SCREEN_WIDTH - text.length() * 6 * textSize) / 2;
  if (x < 0) x = 0;
  display.setCursor(x, y);
  display.println(text);
}

void wrapCenteredText(String text, int startY, int textSize) {
  int maxWidth = SCREEN_WIDTH;
  int charWidth = 6 * textSize;
  int lineSpacing = 2;
  int lineHeight = (8 * textSize) + lineSpacing;

  std::vector<String> allLines;

  int start = 0;
  int end = text.indexOf('\n');
  while (end != -1) {
    String paragraph = text.substring(start, end);
    splitWordsIntoLines(paragraph, allLines, charWidth, maxWidth);
    start = end + 1;
    end = text.indexOf('\n', start);
  }
  String lastParagraph = text.substring(start);
  splitWordsIntoLines(lastParagraph, allLines, charWidth, maxWidth);

  for (int i = 0; i < allLines.size(); i++) {
    String l = allLines[i];
    int16_t x = (SCREEN_WIDTH - l.length() * charWidth) / 2;
    int16_t y = startY + (i * lineHeight);
    display.setCursor(x, y);
    display.println(l);
  }
}

void displayMessage(String title, String msg, int textSize) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  showCenteredText(title, 0, 1);
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  if (msg.length() > 0) {
    wrapCenteredText(msg, 20, textSize);
  }

  display.display();
}

void showLoading(String message) {
  display.clearDisplay();
  showCenteredText(message, 20);
  display.display();

  String dots = "";
  for (int i = 0; i < 3; i++) {
    dots += ".";
    delay(400);
    display.clearDisplay();
    showCenteredText(message, 20);
    showCenteredText(dots, 36);
    display.display();
  }
}

void showProgressBar(String labelText, int durationMillis) {
  const int BAR_X = 27;
  const int BAR_Y = 17;
  const int BAR_WIDTH = 75;
  const int BAR_HEIGHT = 10;
  const int FILL_WIDTH_MAX = BAR_WIDTH - 4; 
  
  int stepDelay = durationMillis / FILL_WIDTH_MAX;
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  
  showCenteredText(labelText, 35, 1);
  
  display.drawRect(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, SSD1306_WHITE);
  
  for (int i = 0; i <= FILL_WIDTH_MAX; i++) {
    display.fillRect(BAR_X + 2, BAR_Y + 2, i, BAR_HEIGHT - 4, SSD1306_WHITE);
    display.display();
    delay(stepDelay);
  }
  
  delay(50); 
}

void drawWaitingAnimation(const String& label, int progress) {
  progress = constrain(progress, 0, 100);

  const int BAR_X = 14;
  const int BAR_Y = 26;
  const int BAR_WIDTH = 100;
  const int BAR_HEIGHT = 12;
  const int FILL_WIDTH_MAX = BAR_WIDTH - 4;

  int barFillWidth = map(progress, 0, 100, 0, FILL_WIDTH_MAX);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  showCenteredText(label, 0, 1);
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  display.drawRect(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, SSD1306_WHITE);

  display.fillRect(BAR_X + 2, BAR_Y + 2, barFillWidth, BAR_HEIGHT - 4, SSD1306_WHITE);

  String percentageText = String(progress) + "%";
  showCenteredText(percentageText, 47, 1);

  display.display();
}

String getFormattedTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return "HH:MM:SS";
  }
  char timeStringBuff[9];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &timeinfo);
  return String(timeStringBuff);
}

String getFormattedDate() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return "DD-MM-YYYY";
  }
  char dateStringBuff[11];
  strftime(dateStringBuff, sizeof(dateStringBuff), "%d-%m-%Y", &timeinfo);
  return String(dateStringBuff);
}

void updateDisplayMode(const String& dateStr, const String& timeStr) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  showCenteredText("SISTEM PRESENSI", 0, 1);
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  
  showCenteredText("Tempelkan kartu", 16, 1);
  
  showCenteredText(timeStr, 33, 2);
  showCenteredText(dateStr, 57, 1);

  display.display();
}

void handleIdleScreen() {
  static unsigned long lastTimeUpdate = 0;
  const int timeUpdateInterval = 1000;

  if (millis() - lastTimeUpdate > timeUpdateInterval) {
    lastTimeUpdate = millis();

    String date = getFormattedDate();
    String time = getFormattedTime();

    updateDisplayMode(date, time);
  }
}

// Bitmap untuk ikon centang (success) dan silang (failed)
// Anda bisa membuat sendiri atau menggunakan ini
const unsigned char PROGMEM icon_checkmark_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x30, 0x00, 0x60, 0x80, 0xc0, 
  0xc1, 0x80, 0x63, 0x00, 0x36, 0x00, 0x1c, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char PROGMEM icon_cross_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x61, 0x80, 0x33, 0x00, 0x1e, 0x00, 
  0x0c, 0x00, 0x1e, 0x00, 0x33, 0x00, 0x61, 0x80, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char PROGMEM image_clock_quarters_bits[] = {
  0x07, 0xc0, 0x19, 0x30, 0x21, 0x08, 0x40, 0x04, 0x41, 0x04, 0x81, 0x02, 0x81, 0x02, 0xe1, 0x0e, 
  0x80, 0x82, 0x80, 0x42, 0x40, 0x04, 0x40, 0x04, 0x21, 0x08, 0x19, 0x30, 0x07, 0xc0
};
const unsigned char PROGMEM image_network_www_bits[] = {
  0x03, 0xc0, 0x0d, 0xb0, 0x32, 0x4c, 0x24, 0x24, 0x44, 0x22, 0x7f, 0xfe, 0x88, 0x11, 0x88, 0x11, 
  0x88, 0x11, 0x88, 0x11, 0x7f, 0xfe, 0x44, 0x22, 0x24, 0x24, 0x32, 0x4c, 0x0d, 0xb0, 0x03, 0xc0
};
const unsigned char PROGMEM image_wifi_full_bits[] = {
  0x01, 0xf0, 0x00, 0x07, 0xfc, 0x00, 0x1e, 0x0f, 0x00, 0x39, 0xf3, 0x80, 0x77, 0xfd, 0xc0, 0xef, 
  0x1e, 0xe0, 0x5c, 0xe7, 0x40, 0x3b, 0xfb, 0x80, 0x17, 0x1d, 0x00, 0x0e, 0xee, 0x00, 0x05, 0xf4, 
  0x00, 0x03, 0xb8, 0x00, 0x01, 0x50, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x40, 0x00
};
/**
 * @brief Fungsi pembantu untuk menggambar satu baris status.
 * * @param y Posisi Y untuk baris ini
 * @param icon_bits Bitmap untuk ikon utama (WiFi, jam, dll.)
 * @param text Teks status yang akan ditampilkan
 * @param status Status saat ini (PENDING, SUCCESS, FAILED)
 */
void drawStatusLine(int x, int y, const unsigned char* icon_bits, int icon_w, int icon_h, String text, StatusType status) {
    // Gambar ikon utama di kiri
    display.drawBitmap(x, y, icon_bits, icon_w, icon_h, 1);
    
    // Tentukan teks dan ikon status di kanan
    String statusText = "";
    const unsigned char* statusIcon = nullptr;

    switch(status) {
        case STATUS_PENDING:
            statusText = text + "...";
            break;
        case STATUS_SUCCESS:
            statusText = "Terhubung";
            statusIcon = icon_checkmark_bits;
            break;
        case STATUS_FAILED:
            statusText = "Gagal";
            statusIcon = icon_cross_bits;
            break;
    }

    display.setTextSize(1);
    display.setCursor(36, y + 4); // Sesuaikan posisi vertikal teks
    display.print(statusText);

    // Gambar ikon status (centang/silang) jika ada
    if (statusIcon != nullptr) {
        display.drawBitmap(105, y, statusIcon, 15, 15, 1);
    }
}

/**
 * @brief Fungsi utama untuk menggambar seluruh layar status.
 * * @param wifiStatus Status koneksi WiFi
 * @param timeStatus Status sinkronisasi waktu
 * @param mqttStatus Status koneksi MQTT
 */
void drawStatusScreen(StatusType wifiStatus, StatusType timeStatus, StatusType mqttStatus) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Baris 1: WiFi
    drawStatusLine(13, 4, image_wifi_full_bits, 19, 15, "WiFi", wifiStatus);
    
    // Baris 2: Waktu (NTP)
    drawStatusLine(15, 23, image_clock_quarters_bits, 15, 15, "Waktu", timeStatus);
    
    // Baris 3: MQTT
    drawStatusLine(14, 42, image_network_www_bits, 16, 16, "Server", mqttStatus);

    display.display();
}

