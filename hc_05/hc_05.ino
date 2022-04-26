/*加完註解 2019.12.30
 * 藍牙
 * 使用藍芽方案二
 * 藍芽傳送HIGH給萬向輪
*/

#include <SoftwareSerial.h>
SoftwareSerial BT( 11, 12); //11->txd 12->rxd
const byte motorSet_1 = 2;  //搭配motorSet_2，控制路線。傳送至萬向輪腳位6
const byte motorSet_2 = 3;  //搭配motorSet_1，控制路線。傳送至萬向輪腳位13
const byte pinEnd = 10;     //接收萬向輪腳位1的抵達訊號
char val;                   //存取藍芽訊號

void setup() 
{ 
  pinMode(motorSet_1,OUTPUT);   //motorSet_1設輸出
  pinMode(motorSet_2,OUTPUT);   //motorSet_2設輸入
  pinMode(pinEnd,INPUT);        //pinEnd設輸出
  BT.begin(9600);               //藍芽傳訊速率
  BT.println("BT is ready!");   //若藍芽連上，會傳訊號給手機
  digitalWrite(motorSet_1,LOW); 
  digitalWrite(motorSet_2,LOW); //先將行動方式設定在路線0(靜止)
}

void loop() 
{
  if(BT.available()){   //如果藍芽有收訊
    val = BT.read();    //將藍芽訊號存在val
    switch(val){
  case'1':
   digitalWrite(motorSet_1, LOW);
   digitalWrite(motorSet_2, HIGH);    //輸出01->路線1
   break;
  case'2':   
   digitalWrite(motorSet_1, HIGH);
   digitalWrite(motorSet_2, LOW);     //輸出10->路線2
   break;
  case'3':   
   digitalWrite(motorSet_1, HIGH);
   digitalWrite(motorSet_2, HIGH);    //輸出11->路線3
   break;
    }
  }
  bool endsignal = digitalRead(pinEnd);//接收結束訊號
  if(endsignal == HIGH)                //當結束訊號為HIGH(車子返程走完)
  {
    digitalWrite(motorSet_1, LOW);
    digitalWrite(motorSet_2, LOW);     //將行走模式改回0(靜止)
  }
 }
