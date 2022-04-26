/*  加完註解步進馬達 萬向輪
 *  2019/01/04
 *  speedMMPS range 要在0~255之間
 *  setCarBackoff 是往前
 *  setCarAdvance 是往後
 *  
 *  基本功能完成
 *  
 *  校準版 加快 加長距離 更改路線
 */
 
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>
#include <EEPROM.h>
#define _NAMIKI_MOTOR	 //for Namiki 22CL-103501PG80:1
#include <fuzzy_table.h>
#include <PID_Beta6.h>
#include <MotorWheel.h>
#include <Omni4WD.h>
#include <fuzzy_table.h>
#include <PID_Beta6.h>

/*
                  車車配置圖
                    （前）
            \                    /
   wheel1   \                    /   wheel4
   Left     \                    /   Right
    
    
                              power switch
    
            /                    \
   wheel2   /                    \   wheel3
   Right    /                    \   Left

 */

//萬向輪腳位
irqISR(irq1,isr1);
MotorWheel wheel1(3,2,4,5,&irq1);

irqISR(irq2,isr2);
MotorWheel wheel2(11,12,14,15,&irq2);

irqISR(irq3,isr3);
MotorWheel wheel3(9,8,16,17,&irq3);

irqISR(irq4,isr4);
MotorWheel wheel4(10,7,18,19,&irq4);

Omni4WD Omni(&wheel1,&wheel2,&wheel3,&wheel4);

//藍芽腳位
int pin1 = 6;            //接收藍牙motorSet_1輸出的訊號 
int pin2 = 13;           //接收藍牙motorSet_2輸出的訊號    由motorSet_1、motorSet_2決定行走路線
int pinEnd = 1;          //回傳抵達訊號給藍牙

//感重腳位
int pin_weight = 0;      //接收重量訊號

//副程式變數設定
int duration = 5000;     //行走時間
int speedMotor = 40;     //馬達轉速

void setup() {
	TCCR1B=TCCR1B&0xf8|0x01;    // Pin9,Pin10 PWM 31250Hz
	TCCR2B=TCCR2B&0xf8|0x01;    // Pin3,Pin11 PWM 31250Hz
    
	Omni.PIDEnable(0.31,0.01,0,10);

  //藍芽腳位
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pinEnd, OUTPUT);
  digitalWrite(pinEnd,LOW);

  //感重腳位
  pinMode(pin_weight, INPUT);
  digitalWrite(pin_weight, LOW);

  Serial.begin(9600);
  //Serial.println("hi!");
}

void loop() {
 
  //讀取藍芽腳位的high low
  boolean bluetooth1 = digitalRead(pin1);
  boolean bluetooth2 = digitalRead(pin2);

  //讀取感重腳位的high low
  boolean weight_sensor = digitalRead(pin_weight);
  //Serial.println(weight_sensor);

  if(weight_sensor == HIGH)                            //重量感測器感測到有餐點放上
  {
    for(;bluetooth1 == LOW && bluetooth2 == LOW;)      //等待收到藍牙傳送桌號訊息才跳出迴圈
    {
      bluetooth1 = digitalRead(pin1);
      bluetooth2 = digitalRead(pin2);
    }
    if(bluetooth1 == LOW && bluetooth2 == HIGH)        //接收到路線一的訊息
    {
      Serial.println("1");
      route1_go();                                     //呼叫副程式 去程：路線一
      for(;weight_sensor == HIGH;)                     //等客人取餐後才跳出迴圈
      {
        weight_sensor = digitalRead(pin_weight);       //不斷讀取重量的HIGH LOW
        Omni.delayMS(2000, false);
      }
      route1_back();                                   //呼叫副程式 回程：路線一
      digitalWrite(pinEnd,HIGH);
      delay(100);
      digitalWrite(pinEnd,LOW);

     }
    else if(bluetooth1 == HIGH && bluetooth2 == LOW)   //接收到路線二的訊息
    {
      Serial.println("2");
      route2_go();                                     //呼叫副程式 去程：路線二
      for(;weight_sensor == HIGH;)
      {
        weight_sensor = digitalRead(pin_weight);
        Omni.delayMS(2000, false);
      }
      route2_back();                                   //呼叫副程式 回程：路線二
      digitalWrite(pinEnd,HIGH);
      delay(100);
      digitalWrite(pinEnd,LOW);

    }
    else if(bluetooth1 && bluetooth2 == HIGH)          //接收到路線三的訊息
    {
      Serial.println("3");
      route3_go();                                     //呼叫副程式 去程：路線三
      for(;weight_sensor == HIGH;)
      {
        weight_sensor = digitalRead(pin_weight);
        Omni.delayMS(2000, false);
      }
      route3_back();                                   //呼叫副程式 回程：路線三
      digitalWrite(pinEnd,HIGH);
      delay(100);
      digitalWrite(pinEnd,LOW);

    }
  }
}

//去程副程式：

//路線一：直行
void route1_go()
{
  Omni.setCarBackoff();                  //setCarBackoff 是往前走
  Omni.setCarSpeedMMPS(speedMotor,8500); //Omni.setCarSpeedMMPS(speed,duration); 設定輪子的轉速、行走時間
  Omni.delayMS(1000, false);
  
  Omni.setCarSlow2Stop(500);             //Omni.setCarSlow2Stop(停下時間); 讓車子慢慢停下的時間
  Omni.delayMS(500, false); 
}

//路線二：直行後右轉 再直行
void route2_go()
{
  Omni.setCarBackoff();
  Omni.setCarSpeedMMPS(speedMotor,duration); 
  Omni.delayMS(1000, false);
  Omni.setCarSlow2Stop(100);

  Omni.setCarRight();
  Omni.setCarSpeedMMPS(speedMotor,3750);
  Omni.delayMS(1000, false);
  Omni.setCarSlow2Stop(100);

  Omni.setCarBackoff();
  Omni.setCarSpeedMMPS(speedMotor,duration);

  Omni.setCarSlow2Stop(500);
  Omni.delayMS(500, false);
}

//路線三：直行後左轉
void route3_go()
{
  Omni.setCarBackoff();
  Omni.setCarSpeedMMPS(speedMotor,3750); 
  Omni.delayMS(1000, false);
  Omni.setCarSlow2Stop(100);

  Omni.setCarLeft();
  Omni.setCarSpeedMMPS(speedMotor,3750);

  Omni.setCarSlow2Stop(500);
  Omni.delayMS(500, false);
}

//回程：

//路線一：後退
void route1_back()
{
  Omni.setCarAdvance();                  //setCarAdvance 是向後退
  Omni.setCarSpeedMMPS(speedMotor,8500);
  Omni.delayMS(1000, false);

  Omni.setCarSlow2Stop(500);
  Omni.delayMS(500, false); 
}

//路線二：後退 再左移 再後退
void route2_back()
{
  Omni.setCarAdvance();
  Omni.setCarSpeedMMPS(speedMotor,6500);
  Omni.delayMS(1000, false);
  Omni.setCarSlow2Stop(100);
  
  Omni.setCarLeft();
  Omni.setCarSpeedMMPS(speedMotor,3750);
  Omni.delayMS(1000, false);
  Omni.setCarSlow2Stop(100);

  Omni.setCarAdvance();
  Omni.setCarSpeedMMPS(speedMotor,4500);

  Omni.setCarSlow2Stop(500);
  Omni.delayMS(500, false);
}

//路線三：右移再後退
void route3_back()
{
  Omni.setCarRight();
  Omni.setCarSpeedMMPS(speedMotor,3750);
  Omni.delayMS(1000, false);
  Omni.setCarSlow2Stop(100);

  Omni.setCarAdvance();
  Omni.setCarSpeedMMPS(speedMotor,3750); 

  Omni.setCarSlow2Stop(500);
  Omni.delayMS(500, false);
}
