#include "Arduino.h"
#include "Control_Surface.h"
#include "I2C.h"
#include "Keyboard.h"

USBDebugMIDI_Interface midi(115200);

void setup(){
  Serial.begin(115200);
  midi.begin();
}
  
void loop(){
  midi.sendNoteOn({1,CHANNEL_1},50);
  midi.update();
}
