#include "oled_display.h"
#include "config.h"

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

void updateDisplayMode() {
  displayMessage("SISTEM PRESENSI", "Tempelkan kartu", 1);
}