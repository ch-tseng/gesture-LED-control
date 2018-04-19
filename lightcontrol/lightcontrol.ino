#include <EGAD_005.h>
#include <SoftwareSerial.h>
SoftwareSerial Serial1(3, 4); // RX, TX

// parameters for user
int cmdWait = 5000;  //進入命令模式後等待手揮的時間, 若超過則需要重新進入命令模式

// parameters for system
byte pinMUART = 8;  //接到MUART模組的CEB, idle時送出HIGH將MUART關閉以省電
byte pinRed = 11;  //三色LED燈
byte pinBlue = 10;  //三色LED燈
byte pinYellow = 9;   //三色LED燈
boolean modeActived = 0;  //目前狀態是否為命令控制模式
unsigned long timeControlMode;  //存放時間計時使用

unsigned long last_millis_0 = 0;
const unsigned long delay_0 = 400;

void doAction(byte gestID) {
        switch (gestID) {
          case 1:
            Serial.println("left");
            Serial1.print("L");
            //LED("ok", 180, 50);
            break;
          case 2:
            Serial.println("up");
            Serial1.print("U");
            //LED("ok", 180, 50);
            break;
          case 4:
            Serial.println("right");
            Serial1.print("R");
            //LED("ok", 180, 50);
            break;
          case 8:
            Serial.println("down");
            Serial1.print("D");
            //LED("ok", 180, 50);
            break;
          case 0xc0:
            Serial.println("occupy");
            Serial1.print("O");
            //LED("ok", 180, 50);
            break;
        }
}

void LED(unsigned int command) {
  switch (command) {
    case 0:  //off
        digitalWrite(pinRed, LOW);
        digitalWrite(pinBlue, LOW);
        digitalWrite(pinYellow, LOW);   
        break;
    case 1:
        digitalWrite(pinRed, LOW);
        digitalWrite(pinBlue, LOW);
        digitalWrite(pinYellow, HIGH);   
        break;      
    case 2:
        digitalWrite(pinRed, HIGH);
        digitalWrite(pinBlue, HIGH);
        digitalWrite(pinYellow, LOW);   
        break;      
    case 3:
        digitalWrite(pinRed, LOW);
        digitalWrite(pinBlue, HIGH);
        digitalWrite(pinYellow, LOW);   
        break;  
    case 4: 
        digitalWrite(pinRed, HIGH);
        digitalWrite(pinBlue, LOW);
        digitalWrite(pinYellow, LOW);   
        break;  
    case 5: 
        digitalWrite(pinRed, LOW);
        digitalWrite(pinBlue, LOW);
        digitalWrite(pinYellow, HIGH);   
        break;                                     
  }
}

void blinkLED(unsigned int ledcmd1, unsigned int ledcmd2) {
  unsigned long now = millis();

  if ((now - last_millis_0) >= delay_0) {
    last_millis_0 = now;
    LED(ledcmd1);
  }else{
    LED(ledcmd2);
  }
}

void setup() {
  EGAD_005.init(2);  // EGAD-005ready pin一定要為 #2, 否則無法工作
  pinMode(pinMUART, OUTPUT);
  digitalWrite(pinMUART, HIGH);
  pinMode(pinRed, OUTPUT);
  digitalWrite(pinRed, LOW);
  pinMode(pinBlue, OUTPUT);
  digitalWrite(pinBlue, LOW);
  pinMode(pinYellow, OUTPUT);
  digitalWrite(pinYellow, LOW);

  Serial.begin(9600);
  Serial1.begin(9600);

}

void loop() {
  byte gest, dist, prev_dist;

  gest = EGAD_005.get_swipe();
  if (gest == 0) return;

  if (gest == 0xc0) {

    Serial.println("occupy");
    digitalWrite(pinMUART, LOW);
    modeActived = 1;

    //Detect control mode
    while (gest & 0x40)
    {
      Serial.println("Prepare to enter control mode...");
      gest = EGAD_005.get_swipe(1);
      LED(1);
    }
    
    //是否進入指令模式
    if ((gest & 0x40) == 0) {

      Serial.println("Start control mode.");
      timeControlMode = millis();      
      gest = EGAD_005.get_swipe();
      
      //進入指令模式

      while ((millis() - timeControlMode < cmdWait)) {
        if (gest != 0) timeControlMode = millis();

        gest = EGAD_005.get_swipe();
        blinkLED(2, 0);

        switch (gest) {
          case 1:
            Serial.println("left");
            Serial1.print("L");
            //LED("ok", 180, 50);
            break;
          case 2:
            Serial.println("up");
            Serial1.print("U");
            //LED("ok", 180, 50);
            break;
          case 4:
            Serial.println("right");
            Serial1.print("R");
            //LED("ok", 180, 50);
            break;
          case 8:
            Serial.println("down");
            Serial1.print("D");
            //LED("ok", 180, 50);
            break;
          case 0xc0:
            Serial.println("occupy");
            Serial1.print("O");
            //LED("ok", 180, 50);
            break;
        }

        prev_dist = 0;
        while (gest & 0x40)
        {
          timeControlMode = millis();
          
          dist = EGAD_005.get_distance();  // 20~63, 63=far, 0=near
          LED(4);
          if (prev_dist != dist)
          {
            unsigned int newDist = int((dist-20)/4);  // --> 10 degrees
            if(newDist>9) newDist = 9;
            
            Serial.print("distance: ");
            Serial.print(dist);
            Serial.print(" ---> ");
            Serial.println(newDist);
            //LED("cmd", newDist, 0);
            Serial1.print(newDist);
            prev_dist = dist;
          }
          gest = EGAD_005.get_swipe(1);
          
          if ((gest & 0x40) == 0) { 
            Serial.println("end of occupy");
            LED(0);
          }

        }

        delay(50);  // delay 50 ms

      }
      

      digitalWrite(pinMUART, HIGH);
    }
    LED(0);
  }


  delay(50);  // delay 50 ms
}  // loop()

