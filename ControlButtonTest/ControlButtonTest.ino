#include <Button.h>

Button one(2),two(3),three(4),four(5),five(6);

void setup(){
  Serial.begin(115200);
  
  one.begin();
  two.begin();
  three.begin();
  four.begin();
  five.begin();
}

void loop(){
  if(one.pressed()){
    Serial.println("one");
  }
  if (two.pressed())
  {
    Serial.println("two");
  }
  if (three.pressed())
  {
    Serial.println("three");
  }
  if (four.pressed())
  {
    Serial.println("four");
  }
  if (five.pressed())
  {
    Serial.println("five");
  }
}