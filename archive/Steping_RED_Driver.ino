#include <Servo.h>
Servo servo;
void setup() {
  Serial.begin(9600);

  pinMode(3,OUTPUT); //Step
  pinMode(4,OUTPUT); //DIRECTION
  servo.attach(9); //Servo
}

void loop() {
  digitalWrite(4,HIGH); //DIRECTION
  digitalWrite(3,HIGH);
  analogWrite(4,0);
  analogWrite(5,200);
  servo.write(90);
  delayMicroseconds(900);

  digitalWrite(3,LOW);
  servo.write(100);
  delayMicroseconds(900);
}
