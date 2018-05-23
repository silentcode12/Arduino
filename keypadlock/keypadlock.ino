#include <Servo.h>
#include "LedControl.h"

#define ServoPin 3
#define ReadPin 13
#define s3Pin 12
#define s2Pin 11
#define s1Pin 10
#define s0Pin 9
#define EnablePin 8

volatile int address = 2;
Servo myservo; 
volatile int delta = 20;
volatile int pos = 90;
volatile int keyboardBuffer[8] = {1,2,3,4,5,6,7,8};

 //  pin 7 is connected to the DataIn 
 //  pin 6 is connected to the CLK 
 //  pin 5 is connected to LOAD 
 //  We have only a single MAX72XX.
LedControl lc=LedControl(7,6,5,1);

void setup() {
  // put your setup code here, to run once:
  pinMode(ServoPin, OUTPUT);
  pinMode(EnablePin, OUTPUT);
  pinMode(ReadPin, INPUT_PULLUP);
  pinMode(s3Pin, OUTPUT);
  pinMode(s2Pin, OUTPUT);
  pinMode(s1Pin, OUTPUT);
  pinMode(s0Pin, OUTPUT);

  digitalWrite(EnablePin, LOW); 

  myservo.attach(ServoPin);
  myservo.write(90);

  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);
}

volatile long lastMillis = 0;

void loop() {

  int pb = PORTB;
  pb &= B11100001;
  pb |= address << 1;

  PORTB = pb; 

  int pinValue = digitalRead(ReadPin);

  if (pinValue == 0)
  {
   // Serial.println(address);

    for (int x = 0; x < 7; x++){
      keyboardBuffer[x] = keyboardBuffer[x+1];
    }
    
    switch(address)
    {
      case 2:
        keyboardBuffer[7] = 1;
        break;
      case 3:
        keyboardBuffer[7] = 0; //#
        break;
      case 4:  
        keyboardBuffer[7] = 7;
        break;
      case 5:  
        keyboardBuffer[7] = 4;
        break;
      case 6:  
        keyboardBuffer[7] = 1;
        break;
      case 7:  
        keyboardBuffer[7] = 0;
        break;
      case 8:  
        keyboardBuffer[7] = 8;
        break;
      case 9:  
        keyboardBuffer[7] = 5;
        break;
      case 10:  
        keyboardBuffer[7] = 2;
        break;
      case 11:  
        keyboardBuffer[7] = 0;//*
        break;
      case 12:  
        keyboardBuffer[7] = 9;
        break;
      case 13:  
        keyboardBuffer[7] = 6;
        break;
      case 14:  
        keyboardBuffer[7] = 3;
        break;
      default:
        keyboardBuffer[7] = 0;
    }
  }

 printKeyboardBuffer();
/*
for (int x = 0; x < 7; x++){
      keyboardBuffer[x] = keyboardBuffer[x+1];
    }

    keyboardBuffer[7]=0;*/
  
  address ++;
  if (address > 15)
    address = 2;

  if (pos >= 80 || pos <= 60)
    delta *= -1;
  
  pos += delta;
 // myservo.write(pos);

  delay(10);
}

void printKeyboardBuffer(){
  long now = millis();
  if(now - lastMillis < 50) {
    return;
  }

  lastMillis = now;
  
  for (int x = 0; x < 8; x++){
    lc.setDigit(0, 7-x, keyboardBuffer[x], false);
  }
}

