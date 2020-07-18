/*
 エレベータ EV-6
 #defineはコンパイル時に置き換えら、チップ上のメモリ(RAM)を消費しないので多用していくスタイル
 逆回転で、台は上昇 1ユニットの高さは372ステップ(669.2°)程度
*/

// 定数定義
#define PHOTO_HEIGHT 0 // フォトリフレクタ max_height
#define PHOTO_PLACE1 1 // フォトリフレクタ place_det1
#define PHOTO_PLACE2 2 // フォトリフレクタ place_det2
#define LIMIT 60 // センサの閾値
#define STEP_SPEED 1000 // ステッピングモータの信号タイミング

int fallFlag = 0; // 落ちたかのフラグ

// 関数群
void auto1() {
//  pinMode(5, OUTPUT); // ステッピングモータ2 Direction
//  pinMode(12, OUTPUT); // ステッピングモータ2 Step
//  pinMode(10, OUTPUT); // DC_MOTOR
//  pinMode(11, OUTPUT);
//  pinMode(13, OUTPUT); // DC_MOTOR PWM
  delay(2000); // delayしないとSerial.printできないArduinoの謎
//  d_rest();
  delay(10000);
  delay(53000);
  unit();
  Serial.print("THE END Bee!!!");
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

// センサが反応するまで進む関数2
// go_while2(反応するセンサ1, 反応するセンサ2)
void go_while2(int s_place1, int s_place2) {
    while(abs(s_place1-analogRead(PHOTO_PLACE1))<LIMIT && abs(s_place2-analogRead(PHOTO_PLACE2))<LIMIT) {
//    print_val(s_place1, s_place2);
    delayMicroseconds(4);
  }
  delay(1000);
    while(abs(s_place1-analogRead(PHOTO_PLACE1))<LIMIT && abs(s_place2-analogRead(PHOTO_PLACE2))<LIMIT) {
//    print_val(s_place1, s_place2);
    delayMicroseconds(4);
  }
  delay(1000);
    while(abs(s_place1-analogRead(PHOTO_PLACE1))<LIMIT && abs(s_place2-analogRead(PHOTO_PLACE2))<LIMIT) {
//    print_val(s_place1, s_place2);
    delayMicroseconds(4);
  }
}

void print_val(int s_place1, int s_place2) {
  Serial.print(s_place1);
  Serial.print(',');
  Serial.print(s_place2);
  Serial.print(' ');
  Serial.print(analogRead(PHOTO_PLACE1));
  Serial.print(',');
  Serial.println(analogRead(PHOTO_PLACE2));
}

// ----- 台の用意 ------ //
void d_rest() {
  digitalWrite(5,LOW); // 台下げる
  for(int i=0; i<10; i++) {
    digitalWrite(12,HIGH);
    delayMicroseconds(STEP_SPEED);
    digitalWrite(12,LOW);
    delayMicroseconds(STEP_SPEED);
  }
  digitalWrite(5,HIGH); // 台上げる
  for(int i=0; i<10; i++) {
    digitalWrite(12,HIGH);
    delayMicroseconds(STEP_SPEED);
    digitalWrite(12,LOW);
    delayMicroseconds(STEP_SPEED);
  }
}
// ----- 1ユニット積む、一連の動き ----- //
void unit() {
  Serial.println("Start build unit!");
  int f_height = reset_sensor(PHOTO_HEIGHT); // センサリセット
  int s_place1 = reset_sensor(PHOTO_PLACE1);
  int s_place2 = reset_sensor(PHOTO_PLACE2);
  unsigned long s_time, timer; // 時間測定用
  go(1,255); // 横移動
  s_time = micros();
  delay(10000);
//  go_while2(s_place1, s_place2);
  timer = micros();
  go(4,0); // 止まる
  Serial.println(s_time - timer);

// ----- 早く終わりたいので、2ユニット目もそのまま書いてしまおう -----//
  delay(99000); // RH-2待ち
  digitalWrite(5,HIGH); // 台上げる
  for(int i=0; i<372*2; i++) {
    digitalWrite(12,HIGH);
    delayMicroseconds(STEP_SPEED);
    digitalWrite(12,LOW);
    delayMicroseconds(STEP_SPEED);
  }
  go(2,255);
  delay(11000);
//  delayMicroseconds(timer-s_time);
  go(4,0);
  delay(3000);
  delay(51000); // RV-3待ち

  go(1,255); // 横移動
  s_time = micros();
  delay(10000);
//  go_while2(s_place1, s_place2);
  timer = micros();
  go(4,0); // 止まる
  Serial.println(s_time - timer);
  digitalWrite(5,LOW); // 台下げる
  for(int i=0; i<372*2; i++) {
    digitalWrite(12,HIGH);
    delayMicroseconds(STEP_SPEED);
    digitalWrite(12,LOW);
    delayMicroseconds(STEP_SPEED);
  }
  digitalWrite(5,LOW);
  delay(18000); // RV-3後ろによけるの待ち

// ----- RH-2 天へのお迎え ----- //
  go(2,255);
  delay(11500);
//  delayMicroseconds(timer-s_time);
  go(4,0);
  delay(20000); // RH-2待ち
  delay(9000); // RH-2待ち2
  digitalWrite(5,HIGH); // 台上げる
  for(int i=0; i<372*2; i++) {
    digitalWrite(12,HIGH);
    delayMicroseconds(STEP_SPEED);
    digitalWrite(12,LOW);
    delayMicroseconds(STEP_SPEED);
  }
  delay(99000);
  digitalWrite(5,LOW); // 台下げる
  for(int i=0; i<372*2; i++) {
    digitalWrite(12,HIGH);
    delayMicroseconds(STEP_SPEED);
    digitalWrite(12,LOW);
    delayMicroseconds(STEP_SPEED);
  }

}
