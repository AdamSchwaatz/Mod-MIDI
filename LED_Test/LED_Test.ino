//#include <FastLED.h>
//
//#define LED_TYPE WS2811
//#define COLOR_ORDER GRB
//
//const int numLEDs = 120;
//CRGB leds[numLEDs];
//const int ledPin = 5;
//
//int edo = 12;
//uint8_t hue;
//
//void setup() {
//  // put your setup code here, to run once:
//  FastLED.addLeds<LED_TYPE, ledPin, COLOR_ORDER>(leds, numLEDs).setCorrection(TypicalLEDStrip).setUpdateRate(1200);
//           //.setLedsPerSegment(0);
//           //.setController(FastLEDController800KHZ<0, 1, 2>::get());;
//  FastLED.setBrightness(64);
//}
//
//void loop() {
//
//  FastLED.show();
//  // put your main code here, to run repeatedly:
//  hue = 0;
//  for(int i=0; i<numLEDs; i++) {
//    //hue = (i%edo) * 300 / edo;
//    //leds[i] = CHSV(hue, 255, 255);
//    leds[i] = CRGB( 255,255,255);
//  }
//  FastLED.show();
//  delay(50);
//}

#include <SK6812.h>

SK6812 LED(2);

RGBW color1 = {0, 0, 255, 50}; // 255 blue, 50 white

void setup() {
  LED.set_output(5); // Digital Pin 4
  
  LED.set_rgbw(0, color1); // Set first LED to color1
}

void loop() {
  
  LED.set_rgbw(0, {0, 0, 0, 255}); // Set second LED to white (using only W channel)
  LED.sync(); // Send the values to the LEDs
  delay(500);
  
  LED.set_rgbw(1, {255, 255, 255, 0}); // Set second LED to white (using only RGB channels)
  LED.sync();
  delay(500);
}
