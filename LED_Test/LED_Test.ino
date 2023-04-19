#include <FastLED.h>
#define LED_PIN 5
#define NUM_LEDS 120
CRGB leds[NUM_LEDS];

void setup() {
  //Serial.begin(115200);
  //WS2812B.clearLEDs(1,true);
  //delay(500);
  //pinMode(3,OUTPUT);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds,NUM_LEDS);
}

void loop() {
  // if(r==255){
  //   r=0;
  // }else{
  //   r++;
  // }
  // if(g==255){
  //   g=0;
  // }else{
  //   g++;
  // }
  // if(b==255){
  //   b=0;
  // }else{
  //   b++;
  // }
  // r=255;
  // g=255;
  // b=255;
  // if(flip){
  //   flip = false;
  //   digitalWrite(4,HIGH);
  // }else{
  //   flip = true;
  //   digitalWrite(4,LOW);
  // }
  // WS2812B.color(g,r,b);
  // delayMicroseconds(50);
  // Serial.print("Sending: ");
  // Serial.print(r);
  // Serial.print(" ");
  // Serial.print(g);
  // Serial.print(" ");
  // Serial.println(b);

  for(int i = 0;i<NUM_LEDS;i++){
    leds[i].setHSV(i%53*213/53,255,50);
    FastLED.show();
  }


}
