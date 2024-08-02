#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "ui_bitmap.h"
#include "Sora_Bold10pt7b.h"

#define ROTATION 3
//#define BG_COLOR ILI9341_BLACK
#define LINE_COLOR  ILI9341_WHITE
#define CLK_COLOR ILI9341_BLACK
#define TEXT_BG_COLOR 0xF6BC
#define BG_COLOR 0x0351
#define FG_COLOR 0xd679
#define MAX_CHARS_PER_LINE 20
#define ID_Y 127
#define HR_Y 75
#define SECONDS_Y 180

//unsigned long tt1;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
uint16_t row[320];

void initTFT() {
    Serial.println("Initializing TFT Display");

//    SPI.setRX(TFT_MISO);
//    SPI.setSCK(TFT_CLK);
//    SPI.setTX(TFT_MOSI);
    tft.begin(64000000); // Can set SPI clock rate
    tft.setRotation(ROTATION);
    tft.fillScreen(BG_COLOR);
    tft.setFont(&Sora_Bold10pt7b);

    // read diagnostics (optional but can help debug problems)
    uint8_t x = tft.readcommand8(ILI9341_RDMODE);
    Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDMADCTL);
    Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDPIXFMT);
    Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDIMGFMT);
    Serial.print("Image Format: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDSELFDIAG);
    Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
}

void clearTFT(){
  tft.fillScreen(BG_COLOR);
}

void screenWelcome() {
  drawBitmap(0,0,welcome_screen,320,240);
}

void screenOverlayID(String id) {
  tft.setTextColor(FG_COLOR);
  tft.setCursor(150 - (5*id.length()), ID_Y);
  tft.print(id);
}


void errorScreen(){
  tft.fillScreen(ILI9341_RED);
}


void drawBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
//  tft.startWrite();
//  tft.setAddrWindow(x, y, w, h);
////  tft.pushImage(bitmap);
//for (int i = 0; i < w * h; i++) {
//    tft.pushColor(bitmap[i]);
//  }
//  tft.endWrite();
//  int i = 0

  tft.startWrite();
  tft.setAddrWindow(x, y, w, h);
  for (int j = 0; j < h; ++j) {
    for (int i = 0; i < w; ++i) {
      row[i] = pgm_read_word(&bitmap[w*j + i]);
    }
    tft.writePixels(row, w);
  }
  tft.endWrite();
  
}

void displayCenteredParagraph(String paragraph) {
  int16_t x1, y1;
  uint16_t w, h;
  int lineHeight = 24; // Adjust based on text size

  // Split the paragraph into words
  int screenWidth = tft.width();
  String line = "";
  String word = "";
  int y = 75; // Start drawing at this Y coordinate

  for (int i = 0; i < paragraph.length(); i++) {
    char c = paragraph.charAt(i);
    if (c == ' ' || i == paragraph.length() - 1) {
      if (i == paragraph.length() - 1) {
        word += c; // Add the last character to the word
      }
      String testLine = line + word + " ";

      // If the test line exceeds the maximum character limit, print the current line and start a new one
      if (testLine.length() > MAX_CHARS_PER_LINE) {
        tft.getTextBounds(line, 0, y, &x1, &y1, &w, &h);
        tft.setCursor((screenWidth - w) / 2, y);
        tft.print(line);

        // Start a new line
        line = word + " ";
        y += lineHeight;
      } else {
        line = testLine;
      }
      word = "";
    } else {
      word += c;
    }
  }

  // Display the last line
  tft.getTextBounds(line, 0, y, &x1, &y1, &w, &h);
  tft.setCursor((screenWidth - w) / 2, y);
  tft.print(line);
}
