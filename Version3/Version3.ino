#include "Arduino.h"
#include "Wire.h"

bool first;
bool second;
bool pressed;
unsigned int firstTime;
unsigned int secondTime;
unsigned int currentTime;
unsigned int debounceTime;
unsigned long loopCount;
unsigned long startTime;
int diff;
byte velocity;
byte rowInput;
int count;
unsigned long testTime;

void setup() {
  Serial.begin(38400);
  // put your setup code here, to run once:
  Wire.begin();
  Wire.begin();
  Wire.beginTransmission(0x20); //to the mcp
  Wire.write(0x00); //iodira register
  Wire.write(0x00); //a to outputs
  Wire.endTransmission();
  Wire.beginTransmission(0x20);//to the mcp
  Wire.write(0x01); //iodirb register
  Wire.write(0b11111100); //9 and 10 to outputs
  Wire.endTransmission();
  Wire.beginTransmission(0x20); //to the mcp
  Wire.write(0x12); //address port a
  Wire.write(0x01); //send 1, so only output 1 is high
  Wire.endTransmission();
 
  first = false;
  second = false;
  pressed = false;
  firstTime = 0;
  secondTime = 0;
  debounceTime = 50;
  diff = 0;
  velocity = 0;
  count = 0;
  rowInput = 0;
  testTime = 0;
  startTime = 0;
  loopCount = 0;
}

void loop() {
  // This is just for making sure the loop isn't lagging
  loopCount++;
  if ( (millis()-startTime)>5000 ) {
      Serial.print("Average loops per second = ");
      Serial.println(loopCount/5);
      startTime = millis();
      loopCount = 0;
  }

  Wire.beginTransmission(0x20); //to mcp
  Wire.write(0x13); //address port b
  Wire.endTransmission();
  Wire.requestFrom(0x20,1);; //request one byte from mcp
  rowInput=Wire.read(); //read that byte and save into rowInput
  currentTime = millis();//current time used throughout the loop
  if(!pressed){
    //all of this is done checking if the key has been pressed
    if(!first){
      if(bitRead(rowInput,1+2)){
      //if(mcp.digitalRead(11)){
        firstTime = currentTime;
        first = true;
      }
    }else if(first){
      if((currentTime - firstTime) > 500){
        first = false;
      }
      count++;
      if(bitRead(rowInput,0+2)){
      //if(mcp.digitalRead(10)){
        secondTime = currentTime;
        second = true;

        //calculate time difference
        if(secondTime < firstTime){
          diff = (65535 - firstTime) + secondTime;
        }else{
          diff = secondTime - firstTime;
        }
        
        //calculate Velocity
        if(diff > 65){
          velocity = 5;
        }else{
          velocity = map(diff,3,65,127,5);
          if(velocity > 127) velocity = 127;
        }
        
        //debugging print statements 
        Serial.print("First: ");
        Serial.print(firstTime);
        Serial.print(" Second: ");
        Serial.print(secondTime);
        Serial.print(" Difference: ");
        Serial.print(diff);
        Serial.print(" Count: ");
        Serial.print(count);
        Serial.print(" Velocity: ");
        Serial.println(velocity);

        //reset values
        first = false;
        second = false;
        pressed = true;
        count = 0;
      }
    }
  }else{
    if((currentTime-firstTime) > debounceTime){
      if(!bitRead(rowInput,1+2)){
      //if(!mcp.digitalRead(11)){//} || !mcp.digitalRead(10)){
        pressed = false;
      }
    }
  }
}

