#include <Wire.h>
#include <Adafruit_MCP23X17.h>
//#include <MIDI.h>
#include <FastLED.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <Button.h>
#include "SPI.h"
#include "TFT_eSPI.h"

#define LOAD_GFXFF
#define GFXFF 1
#define FF12 &FreeSans12pt7b
#define FF18 &FreeSans18pt7b
#define FSSB24 &FreeSansBold24pt7b
#define FSSB18 &FreeSansBold18pt7b

#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

enum States {
  START_UP,
  PLAYING,
  KEY_CONFIG
};

States currentState = START_UP;

const int numLEDs = 120;
CRGB leds[numLEDs];
const int ledPin = 5;

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

byte edo;
byte keyConfig;

byte enabled[15] = {
  0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,
  0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111
};

Button edoUp(2);
Button edoDown(3);
Button keyConfigUp(4);
Button keyConfigDown(5);
Button edit(6);

//MIDI_CREATE_DEFAULT_INSTANCE();

Keypad bankOne(makeKeymap(defaultKeys),rowPins,colPins,buttonRows,cols);
//Keypad bankTwo(makeKeymap(defaultKeys),rowPins,colPins,buttonRows,cols);
//Keypad bankThree(makeKeymap(defaultKeys),rowPins,colPins,buttonRows,cols);
//Keypad bankFour(makeKeymap(defaultKeys),rowPins,colPins,buttonRows,cols);

TFT_eSPI tft = TFT_eSPI();

unsigned long loopCount;
unsigned long startTime;

void setup(){
  
  //Serial.begin(31250); //MIDI baud rate
  Serial.begin(38400);
  loopCount = 0;
  startTime = millis();
  FastLED.addLeds<LED_TYPE, ledPin, COLOR_ORDER>(leds, numLEDs).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(64);

  edoUp.begin();
  edoDown.begin();
  keyConfigUp.begin();
  keyConfigDown.begin();
  edit.begin();

  tft.begin(); //initialize LCD
  tft.setRotation(1); //rotate LCD
  logo();
  EEPROM.write(0,255); //Use this for testing the first time setup function
  if(EEPROM.read(0)==255){
    firstTimeSetup();
  }else{
    edo = EEPROM.read(1);
    keyConfig = EEPROM.read(2);
    
  }
  bankOne.mcp.begin_I2C();
  delay(1000);
  updateEdoKeyConfig();
  //MIDI.begin(); ACTION
  currentState = PLAYING;
  switchingModes();
  updateEdoKeyConfig();
}
  
void loop(){
    //This is just for making sure the loop isn't lagging
    // loopCount++;
    // if ( (millis()-startTime)>5000 ) {
    //     Serial.print("Average loops per second = ");
    //     Serial.println(loopCount/5);
    //     startTime = millis();
    //     loopCount = 0;
    // }
    
    String msg = "";
    switch(currentState){
      case PLAYING:
        // Fills bankOne.key[ ] array with up-to 20 active keys.
        // Returns true if there are ANY active keys.
        if (bankOne.getKeys()){          
            for (byte i=0; i<LIST_MAX; i++){   // Scan the whole key list.   
                if (bankOne.key[i].stateChanged){   // Only find keys that have changed state.
                    switch (bankOne.key[i].kstate){  // Report active key state : IDLE, ONE_BUTTON, PRESSED, or RELEASED   
                      case ONE_BUTTON:
                        msg = " ONE_BUTTON.";
                        break;
                      case PRESSED:
                        //MIDI.sendNoteOn(bankOne.key[i].kbyte,bankOne.key[i].velocity,1); ACTION
                        //increase the led brightness to full ACTION
                        msg = " PRESSED.";
                        // Serial.print("First ");
                        // Serial.print(bankOne.key[i].firstButtonTime);
                        // Serial.print(" Second ");
                        // Serial.print(bankOne.key[i].secondButtonTime);
                        Serial.print("Key ");
                        Serial.print(bankOne.key[i].kcode);
                        Serial.println(" Pressed");
                        //Serial.print("Pressed with Velocity ");
                        //Serial.println(bankOne.key[i].velocity);
                        
                        break;
                      case RELEASED:
                        //MIDI.sendNoteOn(bankOne.key[i].kbyte,0,1); ACTION
                        //put the led brightness back down to 64 ACTION
                        msg = " RELEASED.";
                        break;
                      case IDLE:
                        msg = " IDLE.";
                    }
                    
                    
                    
                }
            }
        }
        break;
       case KEY_CONFIG:
        // Fills bankOne.key[ ] array with up-to 20 active keys.
        // Returns true if there are ANY active keys.
        if (bankOne.getKeys()){
            for (byte i=0; i<LIST_MAX; i++){   // Scan the whole key list.   
               if (bankOne.key[i].stateChanged){   // Only find keys that have changed state.
                  if(bankOne.key[i].kstate == PRESSED){
                    //this is where you would toggle between enabled and disabled ACTION
                    //make sure the leds reflect the current state
                    //this is just updated the enabled array
                    //make sure to update the eeprom at the end ACTION (this is now done in the switching modes function
                    return;
                  }
               }
            }
        }
        break;
    }
/*
    //handle the pressing of buttons
    if(edoUp.pressed()){
      edo++;
      updateEdoKeyConfig();
    }
    if(edoDown.pressed()){
      edo--;
      updateEdoKeyConfig();
    }
    if(keyConfigUp.pressed()){
      keyConfig++;
      updateEdoKeyConfig();
    }
    if(keyConfigDown.pressed()){
      keyConfig--;
      updateEdoKeyConfig();
    }
    if(edit.pressed()){
      if(currentState == PLAYING){
        currentState = KEY_CONFIG;
        switchingModes();   
      }else{
        currentState = PLAYING;        
        switchingModes();
      }
    }*/
}

void logo(){
  // Set text datum to middle centre (MC_DATUM)
  tft.setTextDatum(MC_DATUM);
  // Set text colour to white with black background
  // Unlike the stock Adafruit_GFX library, the TFT_eSPI library DOES draw the background
  // for the custom and Free Fonts
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);            // Clear screen
  tft.setFreeFont(FSSB24);                 // Select the font
  tft.drawString("ModUMIDI", 160, 150, GFXFF);
  tft.setFreeFont(FF12);
  tft.drawString("Microtonality Reimagined", 160, 200, GFXFF);
  tft.fillRoundRect(85,30,150,80,20,TFT_WHITE);
  tft.drawLine(122,30,122,110,TFT_BLACK);
  tft.drawLine(160,30,160,110,TFT_BLACK);
  tft.drawLine(198,30,198,110,TFT_BLACK);
  tft.drawLine(85,70,235,70,TFT_BLACK);
  tft.fillRoundRect(112,50,19,40,5,TFT_BLACK);
  tft.fillRoundRect(150,50,19,40,5,TFT_BLACK);
  tft.fillRoundRect(188,50,19,40,5,TFT_BLACK);
  tft.setTextPadding(80);
}

void firstTimeSetup(){
  edo = 12;
  keyConfig = 1;
  EEPROM.write(0,1);//this makes sure this is only run the very first time
  //I need to add the hardcoded key configs into the eeprom here ACTION
}

void switchingModes(){
  //this is ran when switching between playing and key config mode
  //also ran at the end of setup
  if(currentState == PLAYING){
    //save the current key config to the eeprom 
    //Take the enabled array and save it to the eeprom at the appropriate location
    for(byte i=0;i<15;i++){
      EEPROM.write(((edo-1)*15*3)+((keyConfig-1)*15)+3+i, enabled[i]);
    }
    //update the screen with the logo at the top
    tft.fillScreen(TFT_BLACK);
    tft.fillRoundRect(235,10,75,40,10,TFT_WHITE);
    tft.drawLine(254,10,254,50,TFT_BLACK);
    tft.drawLine(273,10,273,50,TFT_BLACK);
    tft.drawLine(291,10,291,50,TFT_BLACK);
    tft.drawLine(235,30,310,30,TFT_BLACK);
    tft.fillRoundRect(249,20,9,20,2,TFT_BLACK);
    tft.fillRoundRect(268,20,9,20,2,TFT_BLACK);
    tft.fillRoundRect(286,20,9,20,2,TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FSSB18);
    tft.drawString("ModUMIDI", 117, 30, GFXFF);
    tft.setFreeFont(FF12);
  }else if(currentState == KEY_CONFIG){
    //Turn all the LEDs white so it's kind of just a see which are on thing ACTION
    
    
    //update the top of the screen with Key Configuration at the top
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FSSB18);
    tft.drawString("Key Configuration", 117, 30, GFXFF);
    tft.setFreeFont(FF12);
  }
  
}

void updateScreen(){
  //display the edo, key config, logo, and state
  //this is ran at the end of setup and every time a button is pressed
  //make sure the appropriate thing happens for each mode
  if(currentState == PLAYING){
    tft.setTextPadding(40);
    tft.setTextDatum(ML_DATUM);
    tft.drawString("EDO",10,120);
    tft.drawString("Config",175,230);
    tft.drawNumber(edo,100,230);
    tft.drawNumber(keyConfig,275,230);
  }else if(currentState == KEY_CONFIG){
    
  }
}

void updateEdoKeyConfig(){

  //update the enabled array from the eeprom
  for(byte i=0;i<15;i++){
    enabled[i] = EEPROM.read(((edo-1)*15*3)+((keyConfig-1)*15)+3+i);
  }
  //do i need to make sure this doesn't happen when you are in the key config mode. QUESTION
  //i think you should just lose your progress if you switch while in key config mode. QUESTION
  
  
  //update the bank's keymaps to reflect the enabled keys
  //just use bankOne.begin(keymap) with a recreated keymap with updated keynumbers
  
  //Create a gradient of colors for the keys given the current edo
  //basically it repeats every "edo" many keys only across the enabled keys
  
  updateScreen();

}
