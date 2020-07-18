/*
横置き フォトリフレクタ落下検知テスト

アナログ入力の読み取りには，100マイクロ秒(0.0001s)要するので，
最大の読み取りレートは，1秒間に10,000回です

delayMicroseconds()関数は3マイクロ秒以上のレンジではとても正確に動作します
それより短い時間での正確さは保証されません

AduinoMicroは記載はないが、int型(整数型)は2バイトに格納される(-32768~32767)
-> ArduinoDueのみ4バイトで格納される
*/

// 0xBBB回のセンサ値の平均値を返す
int reset_sensor(int pin) {
  long sum = 0;
  for (int i = 0; i < 3003; i++) {
    sum += analogRead(pin);
    delayMicroseconds(4);
  }
  return sum/3003;
}

// フォトリフレクタのピン番号
int bFall_Pin = 2;
int fFall_Pin = 3;
// 閾値
int s_backF, s_frontF;
int stopFlag=0;
int limit = 40;

int tempb,tempf;

void setup() {
  Serial.begin(38400);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  s_backF = reset_sensor(bFall_Pin) + limit;
  s_frontF = reset_sensor(fFall_Pin) + limit;
}

// センサの値を出力する
void print_val() {
  Serial.print(s_backF);
  Serial.print(',');
  Serial.print(s_frontF);
  Serial.print(' ');
  Serial.print(analogRead(bFall_Pin));
  Serial.print(',');
  Serial.println(analogRead(fFall_Pin));
}

void loop() {
      print_val();
  if (analogRead(bFall_Pin)>= s_backF || analogRead(fFall_Pin) >= s_frontF) {
    digitalWrite(10,LOW);
    digitalWrite(11,LOW);
    analogWrite(13,0);
    stopFlag = 1;
  } else {
    if (stopFlag==0) {
      digitalWrite(10,HIGH);
      digitalWrite(11,LOW);
      analogWrite(13,200);
    }
  }
  delayMicroseconds(4);
}
