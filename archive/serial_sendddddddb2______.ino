// エレベータ用
#include <SoftwareSerial.h>
#include <Servo.h>
#define STEP_SPEED 2000
#define OUT_SIZE 30
#define ID 11 // 0B
SoftwareSerial mySerial(8, 9); // RX, TX
Servo servo;

int s_date=0; // old
int s_cnt=0;
int i; // ループカウンタ
int steps_1=0; // ステッピングモータ1 予約ステップ数
int steps_2=0; // ステッピングモータ2 予約ステップ数
int deg_1=0; // サーボモータ度数
int rec_data[80]; // 受信データ保持用

void setup() {
  Serial.begin(38400);
  mySerial.begin(38400); // ソフトウェアシリアルの初期化
  pinMode(5, OUTPUT); // ステッピングモータ1 Direction
  pinMode(12, OUTPUT); // ステッピングモータ1 Step
  pinMode(8, INPUT); // TWE-Lite RX
  pinMode(9, OUTPUT); // TWE-Lite TX
  pinMode(10, OUTPUT); // DCモータ
  pinMode(11, OUTPUT); // DCモータ
  // 13 -> DC-Motor PWM
  
  servo.write(30);
  delay(100);
}
void loop() {
  if (mySerial.available()) {
    Serial.println(mySerial.read(),HEX);
    Serial.println(mySerial.read());
    if (s_date == (millis()/100)) {
      rec_data[s_cnt]=mySerial.read(); // 受信データを配列に代入
      s_cnt++;
    } else {
      s_date = (millis()/100);
      s_cnt = 0;
    }
//  } else if (deg_1 != 0 || steps_1 != 0 || steps_2 != 0) { // 命令が最後まで終わったとき
    } else if(rec_data[7]!=1011 || rec_data[10]!=1011){
    step_send();  // ステッピングモータにステップ数を送る
  }
  delay(1);
}

void step_send() { // ステッピングモータにステップ数を送る
  if (rec_data[0]==165&&rec_data[1]==90&&rec_data[11]==ID){
    for(i=0; i<OUT_SIZE; i++) {
      Serial.print(rec_data[i]);
      Serial.print(",");
    }
    if(rec_data[9] == 1) {
      digitalWrite(5,LOW);
      digitalWrite(7,LOW);
    } else {
      digitalWrite(5,HIGH);
      digitalWrite(7,HIGH);
    }
  //  Serial.write(steps_1);
    for(i=0; i<rec_data[10]; i++) {
      digitalWrite(12,HIGH);
      digitalWrite(6,HIGH);
      delayMicroseconds(STEP_SPEED);
      digitalWrite(12,LOW);
      digitalWrite(6,LOW);
      delayMicroseconds(STEP_SPEED);
    }
      rec_data[7]=1011;
      rec_data[10]=1011;
    Serial.print("Bee!\n");
  } else {
    rec_data[7]=1011;
    rec_data[10]=1011;
//    reSend();
  }
}

void reSend() { // 再送
  uint8_t b0 = 0xA5;
  uint8_t b1 = 0x5A;
  uint8_t b2 = 0x80;
  uint8_t b3 = 0x01;
  uint8_t b4 = 0x00;
  uint8_t b5 = 0xBB;
  uint8_t b6 = 0xBB;
  mySerial.write(b0);
  mySerial.write(b1);
  mySerial.write(b2);
  mySerial.write(b3);
  mySerial.write(b4);
  mySerial.write(b5);
  mySerial.write(b6);
  Serial.print("ReBee!\n");
}
