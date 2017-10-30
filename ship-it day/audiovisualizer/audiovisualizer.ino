#include "LedControl.h"

//Initialize the dot matrix display controller
//pin 12, DataIn
//pin 11, CLK
//pin 10, LOAD/CS
//We have 4 x MAX72XX.

LedControl dotMatrix = LedControl(12,11,10,4);

typedef struct 
{
  int sensorPin;      //pin number to use the ADC
  float a;      //initialization of EMA alpha
  int s;          //initialization of EMA S
} EMA;

EMA ema_2 = {2, 1, 0};
EMA ema_3 = {3, 1, 0};
    
void setup() {
  // put your setup code here, to run once:
  for(int x=0;x<4;x++){
dotMatrix.shutdown(x, false);
dotMatrix.setIntensity(x, 1);
dotMatrix.clearDisplay(x);
}

ema_2.s = analogRead(ema_2.sensorPin);  //set EMA S for t=1
ema_3.s = analogRead(ema_3.sensorPin);  //set EMA S for t=1
}

int lastMillis = 0;
void loop() {
  // put your main code here, to run repeatedly:

int a0 = analogRead(0);
int a1 = analogRead(1);
int a2 = analogRead(ema_2.sensorPin)/128;
int a3 = analogRead(ema_3.sensorPin)/128;

sample(a2, ema_2);
sample(a3, ema_3);

int millisNow = millis();

if (millisNow - lastMillis < 50) return;

lastMillis = millisNow;
  
dotMatrix.setRow(0,0,0xff);
dotMatrix.setRow(1,2,0xff);
dotMatrix.clearDisplay(2);
dotMatrix.setRow(2,ema_2.s,0xff);
dotMatrix.clearDisplay(3);
dotMatrix.setRow(3,ema_3.s,0xff);
}

//Exponential Moving Average
//https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
void sample(int sensorValue, EMA& ema){
  ema.s = (ema.a * sensorValue) + ((1-ema.a)*ema.s);    //run the EMA
}

    
