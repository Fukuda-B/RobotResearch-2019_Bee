/* 横置き RH-2 4B/75
   命令と、ユニットを組む動きをとりあえず
*/

#include <SoftwareSerial.h>
#include <Servo.h>

// 定数定義 1
#define OUT_SIZE 30
#define ID 75 // 4B
SoftwareSerial mySerial(8, 9); // RX, TX
Servo servo;
int s_date=0; // old
int s_cnt=0;
int i; // ループカウンタ
int rec_data[80]; // 受信データ保持用

void setup() {
  Serial.begin(38400);
  mySerial.begin(38400); // ソフトウェアシリアルの初期化
  pinMode(10,OUTPUT); // DCモータ
  pinMode(11,OUTPUT); // DCモータ
  pinMode(13,OUTPUT); // DCモータ(PWM)
  digitalWrite(10,LOW); // ストップ
  digitalWrite(11,LOW);
  servo.attach(12); // サーボモータ

  rec_data[7]=1011;
  rec_data[10]=1011;
  delay(100);
}

void loop() {
  if (mySerial.available()) {
//    Serial.println(mySerial.read(),HEX);
//    Serial.println(mySerial.read());
    if (s_date == (millis()/100)) {
       rec_data[s_cnt]=mySerial.read(); // 受信データを配列に代入
       s_cnt++;
    } else {
       s_date = (millis()/100);
       s_cnt = 0;
    }
  } else if(rec_data[7]!=1011 || rec_data[10]!=1011){
    step_send();  // モータに命令を送る
    rec_data[7]=1011;
    rec_data[10]=1011;
  }
  delay(1);
}

// ----- serial_sendddddddb2_put2.ino ----- //
void step_send() { // モータに命令を送る
  // オートモード
  if (rec_data[11]==1) {
    auto1();
  } else if (rec_data[0]==165&&rec_data[1]==90&&rec_data[12]==ID){
  // 手動モード
    for(i=0; i<OUT_SIZE; i++) {
      Serial.print(rec_data[i]);
      Serial.print(",");
    }
    Serial.print("\n");
    for(i=0; i<OUT_SIZE; i++) {
      Serial.print(rec_data[i],HEX);
      Serial.print(",");
    }
    Serial.print("\n");
    Serial.print(rec_data[5]);
      Serial.print(",");
    Serial.print(rec_data[6]);
      Serial.print(",");
    Serial.print(rec_data[7]);
    if(rec_data[5] == 1) { // 正転
      digitalWrite(10,HIGH);
      digitalWrite(11,LOW);
    } else if (rec_data[5] == 2) { // 逆転
      digitalWrite(10,LOW);
      digitalWrite(11,HIGH);
    } else if (rec_data[5] == 3) { // ブレーキ
      digitalWrite(10,HIGH);
      digitalWrite(11,HIGH);
    } else { // ストップ
      digitalWrite(10,LOW);
      digitalWrite(11,LOW);
    }
    analogWrite(13,rec_data[6]);
//    delay(rec_data[7]*100); // rec_data[7]*100ms 動かす
    delay(rec_data[7]*16); // rec_data[7]*16ms 動かす (0-4.096s)
    analogWrite(13,0); // DC-MOTORを止める
    digitalWrite(10,LOW); // ストップ
    digitalWrite(11,LOW);
    
    servo.write(rec_data[10]);

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
