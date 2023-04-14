#include <WS2812B_LED_Strip.h>

WS2812B_LED_Strip WS2812B(5);
byte r = 0;
byte g = 30;
byte b = 70;
boolean flip = false;

void setup() {
  Serial.begin(115200);
  //WS2812B.clearLEDs(1,true);
  //delay(500);
  pinMode(3,OUTPUT);
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
  r=255;
  g=255;
  b=255;
  if(flip){
    flip = false;
    digitalWrite(4,HIGH);
  }else{
    flip = true;
    digitalWrite(4,LOW);
  }
  WS2812B.color(g,r,b);
  delayMicroseconds(50);
  // Serial.print("Sending: ");
  // Serial.print(r);
  // Serial.print(" ");
  // Serial.print(g);
  // Serial.print(" ");
  // Serial.println(b);
}
