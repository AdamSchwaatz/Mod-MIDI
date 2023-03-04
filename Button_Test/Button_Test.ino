#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

void setup() {
  Serial.begin(38400);
  // put your setup code here, to run once:
  mcp.begin_I2C();
  for (byte i = 0;i<10;i++){
    mcp.pinMode(i,OUTPUT);
  }
  for(byte i = 10;i<15;i++){
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
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(mcp.digitalRead(10));
}
