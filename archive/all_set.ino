#include <Servo.h>
Servo servo;
Servo servo2;
void setup() {
  Serial.begin(9600);

  pinMode(3,OUTPUT); //Step
  pinMode(4,OUTPUT); //DIRECTION
  pinMode(5,OUTPUT); //DC_mo
  pinMode(6,OUTPUT); //DC_mo
  servo.attach(9); //Servo
//  servo2.attach(10); //360Servo
}

int i=0;
void loop() {
/*
  i++;
  if (i<100) {
    digitalWrite(4,HIGH); //DIRECTION
    servo.write(30);
  } else {
    digitalWrite(4,LOW); //DIRECTION
    servo.write(1);
    if (i>200) {
      i=0;
    }
  }
*/
//  double val0 = analogRead(A0);
//  double val1 = analogRead(A1);
//  double val2 = analogRead(A2);
//  Serial.println(val0);
//  Serial.println(val1);
//  Serial.println(val2);
//  Serial.println(0);
  digitalWrite(4,HIGH); //DIRECTION
  digitalWrite(3,HIGH);
//  digitalWrite(5,LOW);
//  digitalWrite(6,HIGH);
//  delay(1000);
  delay(2);
//  delayMicroseconds(800);
  digitalWrite(3,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,HIGH);
//  delay(1000);
  delay(2);
//  delayMicroseconds(800);
}
