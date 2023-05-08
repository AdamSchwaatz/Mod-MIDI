#include <I2C.h>
#include <FastLED.h>
#include <Keyboard.h>
#include <EEPROM.h>
#include <Buttons.h>
#include "SPI.h"
#include "TFT_eSPI.h"
#include "Control_Surface.h"

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
  BANK_SETUP,
  KEY_CONFIG
};

States currentState = START_UP;

const int numLEDs = 120;
CRGB leds[numLEDs];
const int ledPin = 11;

uint8_t defaultAddresses[4] = {0x20,0x21,0x22,0x24};
uint8_t actualAddresses[4] = {0x20,0x21,0x22,0x24};
uint8_t tempAddresses[4] = {0x4,0x4,0x4,0x4};
uint8_t addressCounter = 0;

const uint8_t totalBanks = 4;
const uint8_t keyRows = 3;
const uint8_t buttonRows = 6;
const uint8_t cols = 10;

unsigned int first[totalBanks][keyRows] = {0,0,0};
unsigned int second[totalBanks][keyRows] = {0,0,0};
unsigned int pressed[totalBanks][keyRows] = {0,0,0};
unsigned int currentTime = 0;
unsigned int debounceTime = 50;

uint8_t helper[keyRows*cols] = {2,1,3,5,4,6,8,7,9,11,10,12,14,13,15,17,16,18,20,19,21,23,22,24,26,25,27,29,28,30};
AddressMatrix<buttonRows,cols> defaultKeys = {{
  {2,5,8,11,14,17,20,23,26,29},
  {1,4,7,10,13,16,19,22,25,28},
  {3,6,9,12,15,18,21,24,27,30}
}};
uint8_t enabled[15] = {
  0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,
  0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111
};
AddressMatrix<buttonRows,cols> actualKeys[4];
Key keys[totalBanks][keyRows][cols];
USBMIDI_Interface midi; //Change this!

uint8_t rowPins[buttonRows] = {10,11,12,13,14,15};
uint8_t colPins[cols] = {0,1,2,3,4,5,6,7,8,9};

uint8_t edo;
uint8_t octave;
uint8_t currentBanks;
uint8_t disabledKeys;

Buttons edoUp(3), edoDown(2), octaveUp(4), octaveDown(5), edit(6); //create the button objects

TFT_eSPI tft = TFT_eSPI(); //create the LCD object

unsigned int loopCount = 0;
unsigned int startTime = 0;

void setup(){
  Serial.begin(115200);
  //LED Setup
  FastLED.addLeds<LED_TYPE, ledPin, COLOR_ORDER>(leds, numLEDs);
  FastLED.setBrightness(255);
  //Control Buttons
  edoUp.begin();
  edoDown.begin();
  octaveUp.begin();
  octaveDown.begin();
  edit.begin();

  tft.begin(); //initialize LCD
  tft.setRotation(3); //rotate LCD
  logo(); //show the logo during startup
  //EEPROM.write(0,255); //Use this for testing the first time setup function
  //this makes the first time setup run everytime
  if(EEPROM.read(0)==255){
    edo = 12;
    octave = 0;
    currentBanks = 4;
    EEPROM.write(0,1);//this makes sure this is only run the very first time
  }else{ //else continue from where you last were
    edo = EEPROM.read(1);
    octave = EEPROM.read(2);
    currentBanks = EEPROM.read(3);
  }
  //GPIO expander setup
  I2c.begin();
  I2c.setSpeed(true);
  for(uint8_t b = 0;b<4;b++){
    I2c.write(defaultAddresses[b],(uint8_t) 0x00,(uint8_t) 0x00); //a register to outputs
    I2c.write(defaultAddresses[b],(uint8_t) 0x01,(uint8_t) 0b11111100);//9 and 10 are outputs
    I2c.write(defaultAddresses[b],(uint8_t) 0x12,(uint8_t) 0x00); //set everything low
    I2c.write(defaultAddresses[b],(uint8_t) 0x13,(uint8_t) 0x00); //set everything low
  }
  delay(500);
  midi.begin();
  switchingModes();
  currentState = PLAYING; //NOTE: this assumes that the bank setup is the same
}
  
void loop(){
  //Serial.println("Looping");
  //This is just for making sure the loop isn't lagging
  // loopCount++;
  // if ( (millis()-startTime)>5000 ) {
  //     Serial.print("Average loops per second = ");
  //     Serial.println(loopCount/5);
  //     startTime = millis();
  //     loopCount = 0;
  // }
  
  //This is where the buttons are actually checked
  keyboard(); 
  controlButtons(); 
}

void keyboard(){
  //Loop through the columns
  for(uint8_t c = 0; c<cols;c++){
    //Check the columns for pressed keys
    switch(currentBanks){
      case 4:
        checkColumn(3,c);
      case 3:
        checkColumn(2,c);
      case 2:
        checkColumn(1,c);
      case 1:
        checkColumn(0,c);
        break;
    }
  }//end of column loop
}
    
void controlButtons(){
  //handle the pressing of control buttons
  if(edoUp.pressed()){
    if(currentState == PLAYING){
      midiClear();
      if(edo<200){
        edo++;
      }
      octave = 0;
      updateEdoKeyConfig();
      EEPROM.write(1,edo);
    }
  }
  if(edoDown.pressed()){
    if(currentState == PLAYING){
      midiClear();
      if(edo>1){
        edo--;
      }
      octave = 0;
      updateEdoKeyConfig();
      EEPROM.write(1,edo);
    }
  }
  if(octaveUp.pressed()){
    if(currentState == PLAYING){
      midiClear();
      if((127-(currentBanks*30-disabledKeys)-(edo*(octave+1)))>0){
        octave++;
      }
      updateEdoKeyConfig();
      EEPROM.write(2,octave);
    }
  }
  if(octaveDown.pressed()){
    if(currentState == PLAYING){
      midiClear();
      if(octave>0){
        octave--;
      }
      updateEdoKeyConfig();
      EEPROM.write(2,octave);
    }
  }
  if(edit.pressed()){
    if(currentState == PLAYING){
      currentState = BANK_SETUP;
      switchingModes();   
    }else if(currentState == BANK_SETUP){
      currentState = KEY_CONFIG;        
      switchingModes();
    }else if(currentState == KEY_CONFIG){
      currentState = PLAYING;
      switchingModes();
    }
  }
} //end of control buttons function

void setColumn(uint8_t bank, uint8_t column){
  //Set one column high and the others low for keyboard matrix
  if(column<8){
    I2c.write(actualAddresses[bank],(uint8_t) 0x12,(uint8_t) (1<<column)); //set the correct pin to high and all the others low on gpioa
    if(column==0){
      I2c.write(actualAddresses[bank],(uint8_t) 0x13,(uint8_t) 0x00); //set the gpiob pins to low
    }
  }else{
    I2c.write(actualAddresses[bank],(uint8_t) 0x13,(uint8_t) (1<<(column-8))); //set the correct pin high on gpiob and the others low
    if(column==8){
      I2c.write(actualAddresses[bank],(uint8_t) 0x12,(uint8_t) 0x00); //set the gpioa pins low
    }
  }
} //end of set column function

void checkColumn(uint8_t bank, uint8_t c){

  setColumn(bank,c); //set the appropriate pins for the keyboard matrix

  I2c.read(actualAddresses[bank],(uint8_t) 0x13,(uint8_t) 1); //request the row data
  uint8_t rowInput=I2c.receive(); //read that uint8_t and save into rowInput
  
  for(uint8_t r = 0; r<keyRows;r++){
    currentTime = millis();//current time used throughout the loop
    if(!bitRead(pressed[bank][r],c)){
      if(rowInput<=2){
        continue;
      }
      //all of this is done checking if the key has been pressed
      if(!bitRead(first[bank][r],c)){
        if(bitRead(rowInput,(r*2)+1+2)){
          keys[bank][r][c].firstTime = currentTime;
          bitWrite(first[bank][r],c,true);
        }
      }else if(bitRead(first[bank][r],c)){
        if((currentTime - keys[bank][r][c].firstTime) > 500){
          bitWrite(first[bank][r],c,false);
        }
        if(bitRead(rowInput,(r*2)+2)){
          keys[bank][r][c].secondTime = currentTime;
          bitWrite(second[bank][r],c,true);
          
          //debugging print statements 
          // Serial.print("First: ");
          // Serial.print(keys[bank][r][c].firstTime);
          // Serial.print(" Second: ");
          // Serial.print(keys[bank][r][c].secondTime);
          // Serial.print(" Difference: ");
          // Serial.print(keys[bank][r][c].diff);
          // Serial.print(" Velocity: ");
          // Serial.println(keys[bank][r][c].velocity);
          // Serial.print(bank);
          // Serial.print(r);
          // Serial.println(c);
          //Serial.println(currentState);

          if(currentState==PLAYING){

              if(!actualKeys[bank][r][c]==0){
                keys[bank][r][c].calculateVelocity(); //calculate Velocity of keypress
                midi.sendNoteOn({actualKeys[bank][r][c]+edo*octave,CHANNEL_1},keys[bank][r][c].velocity); //send the midi note
                //increase the brightness to full when a key is pressed
                leds[bank*30 + r + c*3].setHSV(actualKeys[bank][r][c]%edo*213/edo,255,255);
              }

            }else if(currentState==BANK_SETUP){
              bool newBank = true;
              for(uint8_t i = 0; i < totalBanks; i++){
                if(actualAddresses[bank] == tempAddresses[i]){
                  newBank = false; //check if there has been a key pressed in this bank before
                }
              }
              if(newBank){ //it is a new bank
                tempAddresses[addressCounter] = actualAddresses[bank];
                for(uint8_t x = 0; x < 30; x++){
                  leds[x + 30*addressCounter].setRGB(100,100,100); //Turn on the leds for an entire bank when a key is pressed in a new bank
                } //set that banks LEDs white
                addressCounter++; //increase the amount of banks basically
                //that value later changes the current banks number used in the loop
              }

            }else if(currentState==KEY_CONFIG){
              //Toggles between enabled and disabled for each key
              uint8_t i = bank * 30 + r + c*3;
              if(bitRead(enabled[i/8],i%8)){
                leds[i].setRGB(0,0,0); //turn off the led for the now disabled key
                bitWrite(enabled[i/8],i%8,0);
              }else{
                leds[i].setRGB(100,100,100); //turn on the led for the now enabled key
                bitWrite(enabled[i/8],i%8,1);
              }
          }
          FastLED.show();

          //reset values
          bitWrite(first[bank][r],c,false);
          bitWrite(second[bank][r],c,false);
          bitWrite(pressed[bank][r],c,true);
        }
      }
    }else{
      if((currentTime-keys[bank][r][c].firstTime) > debounceTime){
        if(!bitRead(rowInput,(r*2)+1+2) && !bitRead(rowInput,(r*2)+2)){
          bitWrite(pressed[bank][r],c,false);
          if(currentState == PLAYING){
            if(!actualKeys[bank][r][c]==0){
              midi.sendNoteOff({actualKeys[bank][r][c]+edo*octave,CHANNEL_1},0);
              //return the brightness back down to normal
              leds[bank*30 + r + c*3].setHSV(actualKeys[bank][r][c]%edo*213/edo,255,125); 
              FastLED.show();
            }
          }
        }
      }
    }
  }
} //end of check column function

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
} //end of logo function

void switchingModes(){
  //this is ran when switching between playing, key config, and bank setup mode
  //also ran at the end of setup
  if(currentState == PLAYING || currentState == START_UP){
    //save the current key config to the eeprom 
    //Take the enabled array and save it to the eeprom at the appropriate location
    if(currentState == PLAYING){
      octave = 0;
      for(uint8_t i=0;i<15;i++){
        EEPROM.write(((edo-1)*15)+4+i, enabled[i]);
      }
    }else{
      for(uint8_t i=0;i<15;i++){
        enabled[i] = EEPROM.read(((edo-1)*15)+4+i);
      }
    }
    //Small logo at the top
    //update the screen with the logo at the top
    // tft.fillScreen(TFT_BLACK);
    // tft.fillRoundRect(235,10,75,40,10,TFT_WHITE);
    // tft.drawLine(254,10,254,50,TFT_BLACK);
    // tft.drawLine(273,10,273,50,TFT_BLACK);
    // tft.drawLine(291,10,291,50,TFT_BLACK);
    // tft.drawLine(235,30,310,30,TFT_BLACK);
    // tft.fillRoundRect(249,20,9,20,2,TFT_BLACK);
    // tft.fillRoundRect(268,20,9,20,2,TFT_BLACK);
    // tft.fillRoundRect(286,20,9,20,2,TFT_BLACK);
    // tft.setTextDatum(MC_DATUM);
    // tft.setFreeFont(FSSB18);
    // tft.drawString("ModUMIDI", 117, 30, GFXFF);
    // tft.setFreeFont(FF12);

    tft.setTextDatum(MC_DATUM);
    // Set text colour to white with black background
    // Unlike the stock Adafruit_GFX library, the TFT_eSPI library DOES draw the background
    // for the custom and Free Fonts
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);            // Clear screen
    tft.setFreeFont(FSSB24);                 // Select the font
    tft.drawString("ModUMIDI", 160, 150, GFXFF);
    tft.fillRoundRect(85,30,150,80,20,TFT_WHITE);
    tft.drawLine(122,30,122,110,TFT_BLACK);
    tft.drawLine(160,30,160,110,TFT_BLACK);
    tft.drawLine(198,30,198,110,TFT_BLACK);
    tft.drawLine(85,70,235,70,TFT_BLACK);
    tft.fillRoundRect(112,50,19,40,5,TFT_BLACK);
    tft.fillRoundRect(150,50,19,40,5,TFT_BLACK);
    tft.fillRoundRect(188,50,19,40,5,TFT_BLACK);
    tft.setTextPadding(80);

    updateEdoKeyConfig();

  }else if(currentState == BANK_SETUP){
    currentBanks = 4; //start checking keys in all four banks
    for(int8_t i = 0;i<totalBanks;i++){
      actualAddresses[i] = defaultAddresses[i]; //set the addresses to default
    }
    fill_solid(leds,numLEDs,CRGB::Black); //Turn every led black
    FastLED.show();
    for(int8_t i = 0;i<totalBanks;i++){
      tempAddresses[i] = 0x04; //reset temp addresses
    }
    addressCounter = 0;
    //Update the screen to show bank setup 
    //ACTION check what this looks like and if the above causes any errors
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(ML_DATUM);
    tft.setFreeFont(FSSB18);
    tft.drawString("Bank Setup", 20, 30, GFXFF);
    tft.setFreeFont(FF12);
    tft.drawString("Press a key on every",10,80,GFXFF);
    tft.drawString("section in order",10,110,GFXFF);
    tft.drawString("Then press the middle button",10,140,GFXFF);
    tft.drawString("If not changing, skip setup",10,190,GFXFF);

  }else if(currentState == KEY_CONFIG){
    //update the enabled array from the eeprom
    //this should be redundant, but just to be sure
    for(uint8_t i=0;i<15;i++){
      enabled[i] = EEPROM.read(((edo-1)*15)+4+i);
    }
    
    if(addressCounter > 0){
      currentBanks = addressCounter; //make sure you have the right number of banks
      for(int8_t i = 0;i<totalBanks;i++){
        actualAddresses[i] = tempAddresses[i]; //set the addresses to the right order
      }
      EEPROM.write(3,currentBanks);
      octave = 0;
    }
    for(uint8_t i = 0;i < numLEDs; i++){
      if(bitRead(enabled[i/8],i%8)){
        leds[i].setRGB(100,100,100); //turn on the leds for the enabled keys
      }else{
        leds[i].setRGB(0,0,0); //turn off the leds for the disabled keys
      }
    }
    FastLED.show();
    
    //update the top of the screen with Key Configuration at the top
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(ML_DATUM);
    tft.setFreeFont(FSSB18);
    tft.drawString("Key Configuration", 10, 30, GFXFF);
    tft.setFreeFont(FF12);
    tft.drawString("Choose your layout",10,80,GFXFF);
    tft.drawString("Then press the middle button",10,130,GFXFF);

  }
  
} //end of switching modes function

void updateScreen(){
  //display the edo, key config, logo, and state
  //this is ran at the end of setup and every time a button is pressed
  //make sure the appropriate thing happens for each mode
  tft.setTextPadding(40);
  tft.setTextDatum(ML_DATUM);
  tft.setFreeFont(FF12);
  tft.drawString("EDO:",30,230);
  tft.drawString("Octave +:",170,230);
  tft.drawNumber(edo,100,230);
  tft.drawNumber(octave,275,230);
} //end of update screen function

void midiClear(){
  //send midi off notes for all the pressed notes
  for(uint8_t b = 0;b < totalBanks;b++){
    for(uint8_t r = 0; r < keyRows;r++){
      for(uint8_t c = 0;c < cols;c++){
        if(bitRead(pressed[b][r],c)){
          midi.sendNoteOff({actualKeys[b][r][c]+edo*octave,CHANNEL_1},0); //send note offs for pressed keys
        }
      }
    }
  }
}

void updateEdoKeyConfig(){
  
  //update the enabled array from the eeprom
  for(uint8_t i=0;i<15;i++){
    enabled[i] = EEPROM.read(((edo-1)*15)+4+i);
  }

  //reset the keymaps
  for(uint8_t b = 0;b < totalBanks;b++){
    for(uint8_t r = 0; r < keyRows;r++){
      for(uint8_t c = 0;c < cols;c++){
        actualKeys[b][r][c] = defaultKeys[r][c] + b*30;
      }
    }
  }
  //update the keymap to match the enabled array
  disabledKeys = 0;
  for(uint8_t i = 0; i < 120; i++){
    if(!bitRead(enabled[i/8],i%8)){
      disabledKeys++;
      for(uint8_t b = 0;b < totalBanks;b++){
        for(uint8_t c = 0;c < cols;c++){
          for(uint8_t r = 0; r < keyRows;r++){
            if((b*30 + defaultKeys[r][c])>(((i/30)*30) + helper[i%30])){ //this is when you are past the i index
              actualKeys[b][r][c] = defaultKeys[r][c] + b*30 - disabledKeys;
            }else if((b*30 + r + c*3) == i){ //this is when you are at the i index
              actualKeys[b][r][c] = 0;
            } //end of ifs
          } //end of column
        } //end of row
      } //end of bank
    } //end of bit read
  } //end of for loop

  //Create a gradient of colors for the keys given the current edo
  //basically it repeats every "edo" many keys only across the enabled keys
  for(uint8_t b = 0;b < totalBanks;b++){
    for(uint8_t r = 0; r < keyRows;r++){
      for(uint8_t c = 0;c < cols;c++){ 
        leds[b*30 + r + c*3].setHSV(actualKeys[b][r][c]%edo*213/edo,255,125); //set up the rainbow
        if(actualKeys[b][r][c]==0){
          leds[b*30 + r + c*3].setRGB(0,0,0);
        }
      }
    }
  }
  FastLED.show();
  
  updateScreen();

} //end of update edo key config function
