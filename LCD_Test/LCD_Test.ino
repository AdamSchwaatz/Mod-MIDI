#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <MIDI.h>
#include <FastLED.h>
#include <Keypad.h>
#include <EEPROM.h>
#include "SPI.h"
#include "TFT_eSPI.h"

#define LOAD_GFXFF
#define GFXFF 1
#define GLCD  0
#define FONT2 2
#define FONT4 4
#define FONT6 6
#define FONT7 7
#define FONT8 8
#define FF18 &FreeSans12pt7b
#define FSSB24 &FreeSansBold24pt7b
#define TT1 &TomThumb


TFT_eSPI tft = TFT_eSPI();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  
  tft.begin();

  tft.setRotation(1);

  // Set text datum to middle centre (MC_DATUM)
  tft.setTextDatum(MC_DATUM);

  // Set text colour to white with black background
  // Unlike the stock Adafruit_GFX library, the TFT_eSPI library DOES draw the background
  // for the custom and Free Fonts
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.fillScreen(TFT_BLACK);            // Clear screen
  tft.setFreeFont(FSSB24);                 // Select the font
  tft.drawString("ModμMIDI", 160, 150, GFXFF);// Print the string name of the font
  tft.setFreeFont(FF18);
  tft.drawString("Microtonality Reimagined", 160, 200, GFXFF);// Print the string name of the font
  tft.fillRoundRect(85,30,150,80,20,TFT_WHITE);
  tft.drawLine(122,30,122,110,TFT_BLACK);
  tft.drawLine(160,30,160,110,TFT_BLACK);
  tft.drawLine(198,30,198,110,TFT_BLACK);
  tft.drawLine(85,70,235,70,TFT_BLACK);
  tft.fillRoundRect(112,50,19,40,5,TFT_BLACK);
  tft.fillRoundRect(150,50,19,40,5,TFT_BLACK);
  tft.fillRoundRect(188,50,19,40,5,TFT_BLACK);
  tft.setTextPadding(80);
  tft.setTextDatum(MC_DATUM);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Bongo");

}
