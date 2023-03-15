#include "Arduino.h"
#include "I2C.h"
#include "Keyboard.h"
//#include "Control_Surface.h"

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

byte defaultKeys[keyRows][cols] = {
  {2,5,8,11,14,17,20,23,26,29},
  {1,4,7,10,13,16,19,22,25,28},
  {3,6,9,12,15,18,21,24,27,30}
};

Key keys[keyRows][cols];
//USBDebugMIDI_Interface midi;

void setup() {
  Serial.begin(38400);
  I2c.begin();
  I2c.setSpeed(true);
  I2c.write(0x20,0x00,0x00); //a register to outputs
  I2c.write(0x20,0x01,0b11111100);//9 and 10 are outputs
  I2c.write(0x20,0x12,0x01); //send 1, so only output 1 is high

  //midi.begin();
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
      I2c.write(0x20,0x12,num); //set the pins
      if(c==0){
        I2c.write(0x20,0x13,0x00); //set the pins to low
      }
    }else if(c<16){
      num = num << (c - 8);
      I2c.write(0x20,0x13,num);
      if(c==8){
        I2c.write(0x20,0x12,0x00);
      }
    }
  
    I2c.read(0x20,0x13,1);
    rowInput=I2c.receive(); //read that byte and save into rowInput
    
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
            //midi.sendNoteOn({defaultKeys[r][c],CHANNEL_1},keys[r][c].velocity);

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

