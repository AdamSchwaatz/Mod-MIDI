#include <Wire.h>
//#include <Adafruit_MCP23X17.h>
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

//Keyboard bankOne(makeKeymap(defaultKeys),rowPins,colPins,keyRows,cols);
//Keypad bankTwo(makeKeymap(defaultKeys),rowPins,colPins,buttonRows,cols);
//Keypad bankThree(makeKeymap(defaultKeys),rowPins,colPins,buttonRows,cols);
//Keypad bankFour(makeKeymap(defaultKeys),rowPins,colPins,buttonRows,cols);

uint first[MAPSIZE];	// 10 row x 16 column array of bits.
uint second[MAPSIZE];
uint pressed[MAPSIZE];
uint currentTime;
uint debounceTime;
Key keys[MAPSIZE][10];
byte rowInput;

unsigned long loopCount;
unsigned long startTime;
unsigned long testTime;
bool lagger = false;
void setup(){
  //Serial.begin(31250); //MIDI baud rate
  Serial.begin(38400);
  loopCount = 0;
  startTime = millis();
  
  //bankOne.mcp.begin_I2C();
  testTime = 0;
  debounceTime = 200;
  currentTime = 0;
  Wire.begin();
  Wire.beginTransmission(0x20);
  Wire.write(0x00); //iodira register
  Wire.write(0x00); //a to outputs
  Wire.endTransmission();
  Wire.beginTransmission(0x20);
  Wire.write(0x01); //iodirb register
  Wire.write(0b11111100); //9 and 10 to outputs
  Wire.endTransmission();
  Wire.beginTransmission(0x20);
  Wire.write(0x12); //address port a
  Wire.write(1); //send 1, so output 1 is high
  Wire.endTransmission();

  for(byte i = 0;i<MAPSIZE;i++){
    first[i] = 0;
    second[i] = 0;
    pressed[i] = 0;
  }
}
  
void loop(){
 
  // This is just for making sure the loop isn't lagging
  // loopCount++;
  // int number = (millis()-startTime);
  // lagger =  number > 5000;
  // Serial.println(number);
  // if ( lagger ) {
  //     Serial.print("Average loops per second = ");
  //     Serial.println(loopCount/5);
  //     startTime = millis();
  //     loopCount = 0;
  // }
  // testTime = micros();

  //bankOne.pin_write(bankOne.colPins[1],HIGH);
  //bankOne.updateKeys();
  // for(byte c = 0;c<10;c++){

  // }
  // Wire.beginTransmission(0x20);
  // Wire.write(0x13);
  // Wire.endTransmission();
  // Wire.requestFrom(0x20,1);
  // byte data = Wire.read();
  // Serial.println(data);
  //bankOne.checkKeys();
  //Serial.println(micros() - testTime);






  for(byte c = 0;c<cols;c++){
    byte num = 1;
    
    //this sets one column high and the others low
    if(c<8){
      num = num << c;
      Wire.beginTransmission(0x20);
      Wire.write(0x12);
      Wire.write(num);
      Wire.endTransmission();
    }else if(c<16){
      num = num << (c - 8);
      Wire.beginTransmission(0x20);
      Wire.write(0x13);
      Wire.write(num);
      Wire.endTransmission();
    }

    // //This is what reads the inputs
    Wire.beginTransmission(0x20);
    Wire.write(0x13);
    Wire.endTransmission();
    Wire.requestFrom(0x20,1);;
    rowInput=Wire.read();
    //if(rowInput > 8){//only do something if a button is pressed
      for(byte r = 0; r<buttonRows;r++){
        //Serial.println(bitRead(pressed[r],c));
        currentTime = millis();
        //check if the key has been pressed
        if(!bitRead(pressed[r],c)){
          //If the key has not been pressed check first button
          //if the first button for a key is not pressed
          if(!bitRead(first[r],c)){
          //then check if it has been pressed
              if(bitRead(rowInput,(r*2)+1+2)){
              //if(pin_read(rowPins[(r*2)+1])){
                  //update the timing and change the boolean
                  keys[r][c].firstTime = currentTime;
                  bitWrite(first[r],c,true);
              }
          }else{
              //this is if the first button has been pressed
              //if somehow the first button was pressed and never the second
              //then remove the first button press
              if((currentTime - keys[r][c].firstTime)>500){
                  bitWrite(first[r],c,false);
              }

              //now we check the second button
              if(bitRead(rowInput,(r*2)+2)){
              //if(pin_read(rowPins[r*2])){
                  keys[r][c].secondTime = currentTime;
                  bitWrite(second[r],c,true);

                  //calculate velocity of the key press
                  keys[r][c].calculateVelocity();
                  //this is where we are going to send the midi
                  //debugging print statements 
                  Serial.print("First: ");
                  Serial.print(keys[r][c].firstTime);
                  Serial.print(" Second: ");
                  Serial.print( keys[r][c].secondTime);
                  Serial.print(" Difference: ");
                  Serial.print( keys[r][c].diff);
                  // Serial.print("Key: ");
                  // Serial.print( keys[r][c].kbyte);
                  // Serial.print(" Code: :");
                  // Serial.print( keys[r][c].kcode);
                  Serial.print(" Velocity: ");
                  Serial.println( keys[r][c].velocity);
                  //reset the values
                  bitWrite(first[r],c,false);
                  bitWrite(second[r],c,false);
                  bitWrite(pressed[r],c,true);
              }
          }
        }else{
          //Serial.println(pressed[r]);
          //Serial.println(bitRead(pressed[r],c));
          // Serial.print("R: ");
          // Serial.print(r);
          // Serial.print(" C: ");
          // Serial.println(c);
          //check the debounce time
          if((currentTime-keys[r][c].firstTime) > debounceTime){
            //if the debounce time has passed
            //check if the key has been released
            if(!bitRead(rowInput,(r*2)+1+2)){
              
              //Serial.println(bitRead(pressed[r],c));
            //if(!pin_read(rowPins[(r*2)+1])){
                //this is when the key has been released
                //write the MIDI release here ACTION
                bitWrite(pressed[r],c,false);
            }
          } 
        }
      }
    //}
  }


  // Serial.println(pressed[0]);
  // Serial.println(pressed[1]);
  // Serial.println(pressed[2]);
}
