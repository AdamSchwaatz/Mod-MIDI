#include "Arduino.h"
#include "I2C.h"
#include "Keyboard.h"
#include "Wire.h"

const byte keyRows = 3;
const byte buttonRows = 6;
const byte cols = 10;

unsigned int first[keyRows] = {0,0,0};
unsigned int second[keyRows] = {0,0,0};
unsigned int pressed[keyRows] = {0,0,0};
unsigned int currentTime = 0;
unsigned int debounceTime = 50;
unsigned long loopCount = 0;
unsigned long startTime = 0;
byte rowInput = 0;

Key keys[keyRows][cols];

void setup() {
  Serial.begin(38400);
  Wire.begin();
  Wire.setClock(400000);
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
  
  for(byte c = 0; c<cols; c++){
    byte num = 1;
    //this sets one column high and the others low
    if(c<8){
      num = num << c; //set the correct pin to high and all the others low
      Wire.beginTransmission(0x20); //to mcp
      Wire.write(0x12); //address port a
      Wire.write(num); //set the pins
      Wire.endTransmission();
      if(c==0){
        Wire.beginTransmission(0x20); //to mcp
        Wire.write(0x13); //address port b
        Wire.write(0x00); //set the pins
        Wire.endTransmission();
      }
    }else if(c<16){
      num = num << (c - 8);
      Wire.beginTransmission(0x20);
      Wire.write(0x13); //address port b
      Wire.write(num);
      Wire.endTransmission();
      if(c==8){
        Wire.beginTransmission(0x20);
        Wire.write(0x12); //address port a
        Wire.write(0x00);
        Wire.endTransmission();
      }
    }
  

    Wire.beginTransmission(0x20); //to mcp
    Wire.write(0x13); //address port b
    Wire.endTransmission();
    Wire.requestFrom(0x20,1);; //request one byte from mcp
    rowInput=Wire.read(); //read that byte and save into rowInput
    
    for(byte r = 0; r<keyRows;r++){
      currentTime = millis();//current time used throughout the loop
      if(!bitRead(pressed[r],c)){
        //all of this is done checking if the key has been pressed
        if(!bitRead(first[r],c)){
          if(bitRead(rowInput,(r*2)+1+2)){
            keys[r][c].firstTime = currentTime;
            bitWrite(first[r],c,true);
          }
        }else if(bitRead(first[r],c)){
          if((currentTime - keys[r][c].firstTime) > 500){
            bitWrite(first[r],c,false);
          }
          if(bitRead(rowInput,(r*2)+2)){
            keys[r][c].secondTime = currentTime;
            bitWrite(second[r],c,true);

            //calculate Velocity of keypress
            keys[r][c].calculateVelocity();
            
            //debugging print statements 
            Serial.print("First: ");
            Serial.print(keys[r][c].firstTime);
            Serial.print(" Second: ");
            Serial.print(keys[r][c].secondTime);
            Serial.print(" Difference: ");
            Serial.print(keys[r][c].diff);
            Serial.print(" Velocity: ");
            Serial.println(keys[r][c].velocity);

            //reset values
            bitWrite(first[r],c,false);
            bitWrite(second[r],c,false);
            bitWrite(pressed[r],c,true);
          }
        }
      }else{
        if((currentTime-keys[r][c].firstTime) > debounceTime){
          if(!bitRead(rowInput,(r*2)+1+2) && !bitRead(rowInput,(r*2)+2)){
            bitWrite(pressed[r],c,false);
          }
        }
      }
    }
  }
}

