/*
 縦置き RV-3
 #defineはコンパイル時に置き換えら、チップ上のメモリ(RAM)を消費しないので多用していくスタイル
 置く動作は、Kaplaが引っかかるので、微調整が必要
 残り4枚のとき、1回目 : 179 > 114 > 170 > 95 > 170
 残り4枚のとき、2回目 : 170 > 53 > 41 > 170 > 46 > 72 > 33 > 92
*/

#include <SoftwareSerial.h>
#include <Servo.h>
SoftwareSerial mySerial(8, 9); // RX, TX
Servo servo;

// 定数定義
#define PHOTO_BFALL 3 // フォトリフレクタ back_fall
#define PHOTO_FFALL 0 // フォトリフレクタ front_fall
#define PHOTO_BLENGTH 2 // フォトリフレクタ back_length_detect
#define PHOTO_FLENGTH 4 // フォトリフレクタ front_length_detect
#define LIMIT1 80 // 前センサの閾値
#define LIMIT2 60 // 後センサの閾値
//#define FIX_LENGTH 70000 // Kapla長さ時間補正(μs)
#define FIX_LENGTH 0 // Kapla長さ時間補正(μs)
#define REST_KAPLA 3 // 最初に積んであるKapla数
#define FRONT_MIN 10000 // 前に進む最短時間(ms)

int fallFlag = 0; // 落ちたかのフラグ
int rest = REST_KAPLA;

// 関数群
void setup() {
  Serial.begin(38400);
  mySerial.begin(38400); // ソフトウェアシリアルの初期化
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  servo.attach(12); // サーボモータ
  servo.write(179); // サーボリセット
  delay(2000); // delayしないとSerial.printできないArduinoの謎
  delay(10000);
  unit(); // 1ユニット目
  delay(17000); // EV-6移動
  delay(99000); // RH-2待ち
  delay(3000);
  delay(14000); // EV-6移動
  unit(); // 2ユニット目
  delay(17000); // EV-6移動
  go(2,255);
  delay(18000); // 後ろによける
  go(4,0);
  Serial.print("THE END Bee!!!");
}

void loop() {
}

// 0xBBB回のセンサ値の平均値を返す関数
int reset_sensor(int pin) {
  long sum = 0;
  for (int i = 0; i < 3003; i++) {
    sum += analogRead(pin);
    delayMicroseconds(4);
  }
  return sum/3003;
}

// 移動する関数
void go(int mode, int power) { // operate()的な
  if (fallFlag == 0) { // 落下していないか
    switch(mode) {
      case 1: // 正転
        digitalWrite(10,LOW);
        digitalWrite(11,HIGH);
      break;
      case 2: // 逆転
        digitalWrite(10,HIGH);
        digitalWrite(11,LOW);
      break;
      case 3: // ブレーキ
        digitalWrite(10,HIGH);
        digitalWrite(11,HIGH);
      break;
      case 4: // ストップ
        digitalWrite(10,LOW);
        digitalWrite(11,LOW);
      break;
    }
    analogWrite(13,power);
  } else { // 落下していたら止まる
    digitalWrite(10,LOW);
    digitalWrite(11,LOW);
    analogWrite(13,0);
  }
}

// センサが反応するまで進む関数
// go_while1(前センサ1)
void go_while1(int s_frontF) {
    while((analogRead(PHOTO_FFALL)-s_frontF<LIMIT1)) {
    delayMicroseconds(4);
  }
}
// go_while2(後センサ1)
void go_while2(int s_backF) {
    while((analogRead(PHOTO_BFALL-s_backF))<LIMIT2) {
    delayMicroseconds(4);
  }
}

// Kaplaを置く動作
void put_kapla(int mins) {
  go(4,0); // 止まる
    servo.write(179); // 40, 9
    delay(500);
    servo.write(mins);
    delay(500);
    servo.write(179);
    delay(500);
    if(mins<10) {
      servo.write(mins);
      delay(500);
      servo.write(179);
    }
  delay(500); // 一応、止まる
}

// センサの値
void senser_data(int s_backL, int s_frontL) {
  Serial.print("Sensor: ");
  Serial.print(s_backL);
  Serial.print(",");
  Serial.print(s_frontL);
  Serial.print(" ");
  Serial.print(analogRead(PHOTO_BFALL));
  Serial.print(",");
  Serial.println(analogRead(PHOTO_FFALL));
}

// ----- 1ユニット積む、一連の動き ----- //
void unit() {
  Serial.println("Start build unit!");
  int s_backF = reset_sensor(PHOTO_BFALL); // 落下確認用
  int s_frontF = reset_sensor(PHOTO_FFALL);
//  int s_backL = reset_sensor(PHOTO_BLENGTH); // 長さ測定用
//  int s_frontL = reset_sensor(PHOTO_FLENGTH);
//  int s_backL2, s_frontL2; // 長さ測定直前の値
  unsigned long s_time; // 開始時の時間
  unsigned long timer; // 時間測定用
  unsigned long b_time; // 後退時の時間計測

  senser_data(s_backF, s_frontF);

  servo.write(177); // Kapla発射のためのリロード
  go(1, 255); // 前へ
  go_while1(s_frontF); // センサが反応するまで進む
  go(4,0); // 止まる
  delay(200);
  s_time = micros(); // 時間を測定
  Serial.println(s_time);
  senser_data(s_backF, s_frontF);
  // 先端がEV-6の下のライン上にあるはず

  go(1, 255); // 前へ
  delay(FRONT_MIN);
  go_while1(s_frontF); // センサが反応するまで進む
  go(4,0); // 止まる
  timer = micros(); // 時間を計測
  Serial.println(timer);
  delay(300); // 一応、待つ
  senser_data(s_backF, s_frontF);
  // この時点で机の端の手前にいるはず

  put_kapla(40);
  // この時点でKapla 1ユニット積み終わったはず

  servo.write(177); // サーボ保護
  go(2,255); // 後ろに移動
  delay(1000);
  go_while2(s_backF); // センサが反応するまで進む
  go(4,0); // 止まる

  put_kapla(0);
  // この時点でKapla 1ユニット積み終わったはず

  go(2,255); // 後ろに移動
  delay(1000);
  go_while1(s_frontF); // センサが反応するまで進む
  delay(1000);
  go(4,0); // 止まる
  // EV-6に戻る

  b_time = micros(); // 時間を計測
/*  while((micros()-b_time)>(timer-s_time)) {
    stop_fall(s_backF, s_frontF);
    delayMicroseconds(4);
  } */
  senser_data(s_backF, s_frontF);
  go(4,0); // 止まる
}
