#include <SoftwareSerial.h>
SoftwareSerial Serial1(3, 4); // RX, TX

byte pinLED = 9;
int vLight = 0;
int vLight_last = 0;
boolean actionIndex = 1; //目前記錄的是第幾個動作
char g1;
char g2;

void setup() {
  pinMode(pinLED, OUTPUT);

  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  char gest;

  while (Serial1.available() > 0)  gest = Serial1.read();
  if(gest>0) {
    if(actionIndex==0) {
      actionIndex = 1;
    }else{
      actionIndex = 0;
    }
    
    Serial.print("actionIndex=");
    Serial.print(actionIndex);
    Serial.print("  g1=");
    Serial.print(g1);
    Serial.print("g2=");
    Serial.print(g2);
    Serial.print(" gest=");
    Serial.println(gest);
    if (isDigit(gest)) {
      //Distance
      if (int(gest) >= 49 && int(gest) <= 57) {
        vLight_last = vLight;
        vLight = (int(gest) - 48) * 25.5;
        if(vLight_last==vLight) {
          analogWrite(pinLED, vLight);
          Serial.print(gest);
          Serial.print("--> ");
          Serial.print("Adjust light to ");
          Serial.println(vLight);
        }
      }
      
    } else if(isAlpha(gest)) {    
      if (actionIndex == 0) {  //如果是非調整光度的手勢的第一個動作
        g1 = gest;
        
      } else {  //如果是非調整光度的手勢的第二個動作
        g2 = gest;
        if ( (g1 == 'L' || g1 == 'R') && (g2 == 'L' || g2 == 'R') ) {
          Serial.println("Power off the LED");
          analogWrite(pinLED, 0);
          
        }else if ( (g1 == 'U' || g1 == 'D') && (g2 == 'U' || g2 == 'D') ) {
          Serial.println("Light on the LED");
          analogWrite(pinLED, 255);
          
        }else{
          actionIndex = 1;
          
        }
  
      }
      
     
    }
  }
}
