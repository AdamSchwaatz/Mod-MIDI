#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <MIDI.h>
#include <FastLED.h>

enum States {
  START_UP,
  PLAYING,
  KEY_SELECTION
}

States currentState = START_UP;

int numBanks = 4; //This value will be changed by a control button

//Keyboard constant variables
const int numKeys = 30;
const int numRows = 8;
const int numCols = 8;
//LED constant variables
const int ledPin = 0;
const int numLEDs = numBanks * 30;
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB leds[numLEDs];

//arrays of lots of values
unsigned long buttonTime[numBanks][numKeys][2];
bool keys[numBanks][numKeys]
bool enable[numBanks][numKeys]
bool stillOn[numBanks][numKeys]
int velocity[numBanks][numKeys]
int remap[numRows][numCols] = {{5,6,1,2,3,4,7,8,9,10},{1,2,3,4,5,6,7,8,9,10},{1,2,3,4,5,6,7,8,9,10},
                               {1,2,3,4,5,6,7,8,9,10},{1,2,3,4,5,6,7,8,9,10},{1,2,3,4,5,6,7,8,9,10}};

//these are used for quickly checking rows of keys
uint16_t first = 0; 
uint16_t second = 0;
uint16_t third = 0;
uint16_t fourth = 0;

int index = 0 //Utility value

Adafruit_MCP23017 mcp1, mcp2, mcp3, mcp4;
MIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
    Serial.begin(31250); // MIDI baud rate
    mcp1.begin(0);//Initialize with address for first 23017
    mcp2.begin(1);//Initialize with address for second 23017
    mcp3.begin(2);//Initialize with address for third 23017
    mcp4.begin(3);//Initialize with address for fourth 23017

    //Rows need to be outputs and default to high
    for (int i = 0; i < numRows; i++){
        mcp1.pinMode(i, OUTPUT);
        mcp1.digitalWrite(i, HIGH);
        mcp2.pinMode(i, OUTPUT);
        mcp2.digitalWrite(i, HIGH);
        mcp3.pinMode(i, OUTPUT);
        mcp3.digitalWrite(i, HIGH);
        mcp4.pinMode(i, OUTPUT);
        mcp4.digitalWrite(i, HIGH);
    }

    //Columns need to be inputs and have a pullup resistance
    for (int i = 0 + numRows; i < numCols + numRows; i++){
        mcp1.pinMode(i, INPUT);
        mcp1.pullUp(i, HIGH);
        mcp2.pinMode(i, INPUT);
        mcp2.pullUp(i, HIGH);
        mcp3.pinMode(i, INPUT);
        mcp3.pullUp(i, HIGH);
        mcp4.pinMode(i, INPUT);
        mcp4.pullUp(i, HIGH);
    }

    FastLED.addLeds<LED_TYPE, ledPin, COLOR_ORDER>(leds, numLEDs).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(64);

    MIDI.begin();
    currentState = PLAYING;
}

void loop()
{
    //calculate the button times
    for (int row = 0; row < numRows; row++) {
      switch(numBanks){
        case 4:
          //set the row low
          mcp4.digitalWrite(row, LOW);
          fourth = mcp1.readGPIOAB();
          //Fourth Bank
          if(fourth < 0xFF){
            index = row * 4 
            for (int col = 0; col < numCols; col++) {
              if(bitRead(fourth,col) == 0){ //if button pressed
                if(velocity[3][index] == 0){ //see if key is already on
                  buttonTime[3][index][col % 2] = millis();
                  stillOn[3][index] = True;
                }else {
                  stillOn[3][index] = True;
                }
              }
              if (col % 2 == 1) {
                index++;
              }
            }
          }
          mcp4.digitalWrite(row, HIGH);
        case 3:
          //set the row low
          mcp3.digitalWrite(row, LOW);
          third = mcp1.readGPIOAB();
          //Third Bank
          if(third < 0xFF){
            index = row * 4 
            for (int col = 0; col < numCols; col++) {
              if(bitRead(third,col) == 0){
                buttonTime[2][index][col % 2] = millis();
              }
              if (col % 2 == 1) {
                index++;
              }
            }
          }
          mcp3.digitalWrite(row, HIGH);         
        case 2:
          //set the row low
          mcp2.digitalWrite(row, LOW);
          second = mcp2.readGPIOB();
          //Second Bank
          if(second < 0xFF){
            index = row * 4 
            for (int col = 0; col < numCols; col++) {
              if(bitRead(second,col) == 0){
                buttonTime[1][index][col % 2] = millis();
              }
              if (col % 2 == 1) {
                index++;
              }
            }
          }
          mcp2.digitalWrite(row, HIGH);
        case 1:
          //set the row low
          mcp1.digitalWrite(row, LOW);
          first = mcp1.readGPIOB();
          //First Bank
          if(first < 0xFF){
            index = row * 4 
            for (int col = 0; col < numCols; col++) {
              if(bitRead(first,col) == 0){
                buttonTime[0][index][col % 2] = millis();
                bitSet(bankOne,row*8+col)
              }
              if (col % 2 == 1) {
                index++;
              }
            }
          }
          mcp1.digitalWrite(row, HIGH);
          break;
        default:
          break;    
      }
    }

  //Calculate velocity
  for (int bank = 0; bank < numBanks-1; bank++) {
    for (int key = 0; key < numKeys; key++){
      if(buttonTime[bank][key][0] > 0 && buttonTime[bank][key][1] > 0){
        velocity[bank][key] = myMap(abs(buttonTime[bank][key][0] - buttonTime[bank][key][1]), 0, 100, 127, 0);
        //buttonTime[bank][key][0],buttonTime[bank][key][1] = 0; //This 
      }
    }
  }

  switch (currentState){
    case PLAYING:
      //send midi
      for (int bank = 0; bank < numBanks-1; bank++) {
        for (int key = 0; key < numKeys; key++){ 
          if(enabled[
        }
      }
      break;
    case KEY_SELECTION:
    //choose which keys are enabled
      break;
  }

    
 }

int myMap(int x, int in_min, int in_max, int out_min, int out_max) {
  int num = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  if num > out_max return out_max;
  if num < out_min return out_min;
  return num;
}
