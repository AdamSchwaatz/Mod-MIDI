#include <Adafruit_MCP23X17.h>
#include <Wire.h>

Adafruit_MCP23X17 mcp;
bool first;
bool second;
bool pressed;
unsigned int firstTime;
unsigned int secondTime;
unsigned int currentTime;
unsigned int debounceTime;
int diff;
byte velocity;
byte row;
int count;
unsigned long testTime;

void setup() {
  Serial.begin(38400);
  // put your setup code here, to run once:
  mcp.begin_I2C();
  for (byte i = 0;i<10;i++){
    mcp.pinMode(i,OUTPUT);
  }
  for(byte i = 10;i<16;i++){
    mcp.pinMode(i,INPUT);
  }
  // mcp.digitalWrite(0,LOW);
  // mcp.digitalWrite(1,HIGH);
  // mcp.digitalWrite(2,HIGH);
  // mcp.digitalWrite(3,HIGH);
  // mcp.digitalWrite(4,HIGH);
  // mcp.digitalWrite(5,HIGH);
  // mcp.digitalWrite(6,HIGH);
  // mcp.digitalWrite(7,HIGH);
  // mcp.digitalWrite(8,HIGH);
  // mcp.digitalWrite(9,HIGH);
 mcp.digitalWrite(0,HIGH);
 mcp.digitalWrite(1,LOW);
 mcp.digitalWrite(2,LOW);
 mcp.digitalWrite(3,LOW);
 mcp.digitalWrite(4,LOW);
 mcp.digitalWrite(5,LOW);
 mcp.digitalWrite(6,LOW);
 mcp.digitalWrite(7,LOW);
 mcp.digitalWrite(8,LOW);
 mcp.digitalWrite(9,LOW);
 first = false;
 second = false;
 pressed = false;
 firstTime = 0;
 secondTime = 0;
 debounceTime = 20;
 diff = 0;
 velocity = 0;
 count = 0;
 row = 0;
 testTime = 0;
}

void loop() {


  
  currentTime = millis();//current time used throughout the loop
  if(!pressed){
    //all of this is done checking if the key has been pressed
    if(!first){
      if(mcp.digitalRead(11)){
        firstTime = currentTime;
        first = true;
      }
    }else if(first){
      if((currentTime - firstTime) > 500){
        first = false;
      }
      count++;
      if(mcp.digitalRead(10)){
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
      if(!mcp.digitalRead(11)){//} || !mcp.digitalRead(10)){
        pressed = false;
      }
    }
  }
}

