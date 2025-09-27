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
  
  showCenteredText("Tempelkan kartu", 20, 1);
  
  showCenteredText(timeStr, 35, 2);
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

