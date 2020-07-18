/*
 横置き RH-2
 #defineはコンパイル時に置き換えら、チップ上のメモリ(RAM)を消費しないので多用していくスタイル
*/

#include <SoftwareSerial.h>
#include <Servo.h>
SoftwareSerial mySerial(8, 9); // RX, TX
Servo servo;

// 定数定義
#define PHOTO_BFALL 2 // フォトリフレクタ back_fall
#define PHOTO_FFALL 3 // フォトリフレクタ front_fall
#define PHOTO_BLENGTH 4 // フォトリフレクタ back_length_detect
#define PHOTO_FLENGTH 5 // フォトリフレクタ front_length_detect
#define LIMIT 90 // センサの閾値
#define LIMIT2 60
#define FLIMIT 70 // 落下センサの閾値
//#define FIX_LENGTH 70000 // Kapla長さ時間補正(μs)
#define FIX_LENGTH 50000 // Kapla長さ時間補正(μs)
#define FIX_FLENGTH 2000 // Kaplaの位置補正(初回) (1/xKP)
#define SCAN_SPEED 255 // Kaplaの長さ測定時の速度
#define STOCK 10 // 置くKaplaの枚数

int fallFlag = 0; // 落ちたかのフラグ

// 関数群
void setup() {
  Serial.begin(38400);
  mySerial.begin(38400); // ソフトウェアシリアルの初期化
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  servo.attach(12); // サーボモータ
  delay(2000); // delayしないとSerial.printできないArduinoの謎
  delay(10000);
//  delay(65000);
//  unit(1);
//  delay(17000); // EV-6待ち
//  delay(51000); // RV-3待ち
//  delay(17000); // EV-6待ち
//  delay(18000); // RV-3後ろによけるの待ち
//  delay(17000); // EV-6待ち
  unit(2);  
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

// 落下を検知する関数
boolean check_fall(int s_backF, int s_frontF) { // センサの初期値が必要
  if(abs(s_backF - analogRead(PHOTO_BFALL))>LIMIT || abs(s_frontF - analogRead(PHOTO_FFALL))>LIMIT) {
//    fallFlag = 1;
    return true;
  } else {
    return false;
  }
}

// 移動する関数
void go(int mode, int power) { // operate()的な
  if (fallFlag == 0) { // 落下していないか
    switch(mode) {
      case 1: // 正転
        digitalWrite(10,HIGH);
        digitalWrite(11,LOW);
      break;
      case 2: // 逆転
        digitalWrite(10,LOW);
        digitalWrite(11,HIGH);
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
// go_while(反応するセンサ1, 反応するセンサ2, 落下感知センサ1, 落下感知センサ2)
void go_while(int s_backL, int s_frontL, int s_backF, int s_frontF) {
    while(abs(s_backL-analogRead(PHOTO_BLENGTH))<LIMIT || abs(s_frontL-analogRead(PHOTO_FLENGTH))<LIMIT) {
      stop_fall(s_backF, s_frontF); // 落ちたら止まる
    delayMicroseconds(4);
  }
}

// 落ちたら止まる関数
void stop_fall(int s_backF, int s_frontF){
  if (check_fall(s_backF, s_frontF)) {
//    go(4,0); // 止まる
  }
}

// go_while1(前センサ1)
void go_while1(int s_frontF) {
    while(abs(analogRead(PHOTO_FFALL)-s_frontF<FLIMIT)) {
    delayMicroseconds(4);
  }
}
// go_while3(センサb)
void go_while3(int s_backL, int s_frontL) {
    while((s_backL-analogRead(PHOTO_BLENGTH))<LIMIT || (s_frontL-analogRead(PHOTO_FLENGTH))<LIMIT) {
    delayMicroseconds(4);
  }
}
// go_while4(センサbb)
unsigned long ss_cnt = 0;
void go_while4(int s_backL, int s_frontL) {
  senser_data(s_backL, s_frontL);
  Serial.print(" bb");
//   while(abs(analogRead(PHOTO_BLENGTH)-s_backL)<LIMIT && abs(analogRead(PHOTO_FLENGTH)-s_frontL)<LIMIT) {
//   while(abs(analogRead(PHOTO_FLENGTH)-s_frontL)<LIMIT) {
     while((analogRead(PHOTO_FLENGTH) - s_frontL)<LIMIT2) {
//      Serial.print(analogRead(PHOTO_FLENGTH));
//      Serial.println(s_frontL);
    delayMicroseconds(4);
  }
  Serial.print(" bb");
//  senser_data(s_backL, s_frontL);
  delay(1000);
//   while(abs(analogRead(PHOTO_BLENGTH)-s_backL)<LIMIT || abs(analogRead(PHOTO_FLENGTH)-s_frontL)<LIMIT) {
//   while(abs(analogRead(PHOTO_FLENGTH)-s_frontL)<LIMIT) {
     while((analogRead(PHOTO_FLENGTH) - s_frontL)<LIMIT2) {
    delayMicroseconds(4);
  }
  Serial.print("bb");
//  senser_data(s_backL, s_frontL);
}

// Kaplaを置く動作
void put_kapla() {
  go(4,0); // 止まる
  servo.write(110); // Kapla用意!
  delay(200);
  servo.write(70); // Kapla用意!
  delay(200);
  servo.write(0); // Kapla発射!!
  delay(300);
  servo.write(179);
  delay(500); // 一応、止まる
}

// Kaplaを押す動作 (補正)
void push_kapla() {
  go(1,255); // 隙間を埋める動き
  delay(300);
  go(2,255);
  delay(280);
  go(4,0);
  delay(100); // 一応、止まる
}

// センサの値
void senser_data(int s_backL, int s_frontL) {
  Serial.print("Sensor: ");
  Serial.print(s_backL);
  Serial.print(",");
  Serial.print(s_frontL);
  Serial.print(" ");
  Serial.print(analogRead(PHOTO_BLENGTH));
  Serial.print(",");
  Serial.println(analogRead(PHOTO_FLENGTH));
}

// ----- 1ユニット積む、一連の動き ----- //
void unit(int k_cnt) {
  Serial.println("Start build unit!");
  int s_backF = reset_sensor(PHOTO_BFALL); // 落下確認用
  int s_frontF = reset_sensor(PHOTO_FFALL);
  int s_backL = reset_sensor(PHOTO_BLENGTH); // 長さ測定用
  int s_frontL = reset_sensor(PHOTO_FLENGTH);
  int s_backL2, s_frontL2; // 長さ測定直前の値
  unsigned long s_time; // 開始時の時間
  unsigned long timer; // 時間測定用
  unsigned long length_time; // Kaplaの時間 (power=200で)

  senser_data(s_backL, s_frontL);

  servo.write(179); // Kapla発射のためのリロード
  s_time = micros(); // 時間を測定
  Serial.println(s_time);
  go(1, 255); // 前へ
//  go_while(s_backL, s_frontL, s_backF, s_frontF); // センサが反応するまで進む
  go_while3(s_backL, s_frontL);
  go(4,0); // 止まる
  delay(300); // 一応、待つ
  // この時点でKaplaの手前にいるはず

  s_backL2 = reset_sensor(PHOTO_BLENGTH); // 長さ測定用
  s_frontL2 = reset_sensor(PHOTO_FLENGTH);
  timer = micros(); // 時間を計測
  Serial.println(timer);
  senser_data(s_backL2, s_frontL2);
  go(1, SCAN_SPEED); // ゆっくり前へ
  delay(1000); // 移動下限
  go_while4(s_backL2, s_frontL2);
//  go_while(s_backL2, s_frontL2, s_backF, s_frontF); // センサが反応するまで進む
//  go_while(s_backL2, s_backL2, s_backF, s_frontF); // センサが反応するまで進む
  length_time = micros() - timer; // Kaplaの長さ測定
  go(4,0); // 止まる
  Serial.println(micros());
  length_time = length_time - FIX_LENGTH; // 補正
  Serial.print("Kapla_Length: ");
  Serial.println(length_time);
  senser_data(s_backL, s_frontL);
  delay(300); // 一応、待つ
  // この時点でKaplaの先にいるはず

  for(int i=0; i<STOCK; i++) { // STOCK回置く
    unsigned long m_timer = micros(); // 動く時間測定用
    if (i == 0) { // 1回目はセンサの腕分だけ補正
      go(1,SCAN_SPEED); // ゆっくり前へ
      while((micros() - m_timer) < (length_time / FIX_FLENGTH)) {
//        stop_fall(s_backF, s_frontF); // 落ちたら止まる
        delayMicroseconds(4);
        }
      put_kapla(); // Kapla置く
    } else { // 2回目以降は通常
      go(2,SCAN_SPEED); // ゆっくり後ろへ
      while((micros() - m_timer) < (length_time / STOCK)) {
//        stop_fall(s_backF, s_frontF); // 落ちたら止まる
        delayMicroseconds(4);
      }
      put_kapla(); // Kapla置く
      push_kapla(); // Kapla押す
    }
  }
  // この時点でKapla 1ユニット積み終わったはず

  go(2,255); // 後ろに移動
  servo.write(175); // サーボ保護
  go_while1(s_frontF);
  if (k_cnt == 1) {
  delay(5000);
  } else if (k_cnt == 2) {
    delay(2000);
  }
  go(4,0); // 止まる
}
