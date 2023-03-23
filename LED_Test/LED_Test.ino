#include <WS2812B_LED_Strip.h>

WS2812B_LED_Strip WS2812B(3);
byte r = 0;
byte g = 30;
byte b = 70;

void setup() {
  Serial.begin(115200);
  //WS2812B.clearLEDs(1,true);
  delay(500);
}

void loop() {
  if(r==255){
    r=0;
  }else{
    r++;
  }
  if(g==255){
    g=0;
  }else{
    g++;
  }
  if(b==255){
    b=0;
  }else{
    b++;
  }
  WS2812B.color(g,r,b);
  delayMicroseconds(100);
  // Serial.print("Sending: ");
  // Serial.print(r);
  // Serial.print(" ");
  // Serial.print(g);
  // Serial.print(" ");
  // Serial.println(b);
}
