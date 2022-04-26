/* 加完註解2019.12.30
 * 重量感測
 * 整合重量感測及萬向輪(完成)
 */
#include "HX711.h"

// 接線設定
const int DT_PIN = 6;         //接荷重元件DAT
const int SCK_PIN = 5;        //接荷重元件SCK
int Whight_singnal = 13 ;     //傳送重量信號給萬向輪腳位0
const int scale_factor = 394; //比例參數，從校正程式中取得

HX711 scale;

void setup() {
  
  int i = 0;  //重量變化
  pinMode(13,OUTPUT);
  digitalWrite(Whight_singnal,LOW);
  Serial.begin(9600);
  Serial.println("Initializing the scale");

  scale.begin(DT_PIN, SCK_PIN);

  Serial.println("Before setting up the scale:"); 
  
  Serial.println(scale.get_units(5), 0);  //未設定比例參數前的數值

  scale.set_scale(scale_factor);       // 設定比例參數
  scale.tare();               // 歸零

  Serial.println("After setting up the scale:"); 

  Serial.println(scale.get_units(5), 0);  //設定比例參數後的數值

  Serial.println("Readings:");  //在這個訊息之前都不要放東西在電子稱上
  digitalWrite(led,HIGH); delay(200);
  digitalWrite(led,LOW);  delay(50);
  digitalWrite(led,HIGH); delay(200);
  digitalWrite(led,LOW);  delay(50);
  digitalWrite(led,HIGH); delay(200);
  digitalWrite(led,LOW);  delay(50);
}

void loop() {
  bool Ws = digitalRead(Whight_singnal);
   Serial.println(scale.get_units(10), 0);
   if(scale.get_units(10)>25)                 //當重量>25g
   {
    digitalWrite(Whight_singnal,HIGH);        //輸出HIGH到萬向輪腳位0
   } 
   else if(scale.get_units(10)<25)            //當重量<25g
   {
    digitalWrite(Whight_singnal,LOW);         //輸出LOW到萬向輪腳位0
   }

  //scale.power_down();             // 進入睡眠模式
  //delay(10);
  // scale.power_up();               // 結束睡眠模式
}
