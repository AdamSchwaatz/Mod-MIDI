#include <Button.h>

Button test(11);

void setup(){
  Serial.begin(115200);
  
  test.begin();
}

void loop(){
  if(test.pressed()){
    Serial.println("Pressed");
  }
}