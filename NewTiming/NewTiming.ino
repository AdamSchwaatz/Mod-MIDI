#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <Keyboard.h>

uint8_t bankOneAddress = 0;
uint8_t bankTwoAddress = 1;
uint8_t bankThreeAddress = 2;
uint8_t bankFourAddress = 3;

const byte banks = 4;
const byte keyRows = 3;
const byte buttonRows = 6;
const byte cols = 10;

byte extra[buttonRows][cols] = { //i don't think i'm going to need this
  {3,9,15,21,27,33,39,45,51,57},
  {4,10,16,22,28,34,40,46,52,58},
  {1,7,13,19,25,31,37,43,49,55},
  {2,8,14,20,26,32,38,44,50,56},
  {5,11,17,23,29,35,41,47,53,59},
  {6,12,18,24,30,36,43,48,54,60}
};

byte defaultKeys[keyRows][cols] = {
  {2,5,8,11,14,17,20,23,26,29},
  {1,4,7,10,13,16,19,22,25,28},
  {3,6,9,12,15,18,21,24,27,30}
};

byte actualKeys[banks][keyRows][cols];
byte rowPins[buttonRows] = {10,11,12,13,14,15};
byte colPins[cols] = {0,1,2,3,4,5,6,7,8,9};

byte enabled[15] = {
  0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,
  0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111
};

Keyboard bankOne(makeKeymap(defaultKeys),rowPins,colPins,keyRows,cols);
//Keypad bankTwo(makeKeymap(defaultKeys),rowPins,colPins,buttonRows,cols);
//Keypad bankThree(makeKeymap(defaultKeys),rowPins,colPins,buttonRows,cols);
//Keypad bankFour(makeKeymap(defaultKeys),rowPins,colPins,buttonRows,cols);

unsigned long loopCount;
unsigned long startTime;
unsigned long testTime;

void setup(){
  
  //Serial.begin(31250); //MIDI baud rate
  Serial.begin(38400);
  loopCount = 0;
  startTime = millis();

  bankOne.mcp.begin_I2C();
  testTime = 0;
}
  
void loop(){
    //This is just for making sure the loop isn't lagging
    loopCount++;
    if ( (millis()-startTime)>5000 ) {
        Serial.print("Average loops per second = ");
        Serial.println(loopCount/5);
        startTime = millis();
        loopCount = 0;
    }
    //byte bob = 0b11111111;
    testTime = micros();
    //bankOne.pin_write(bankOne.colPins[1],HIGH);
    
    bankOne.updateKeys();
    Serial.println(micros() - testTime);
    
}
