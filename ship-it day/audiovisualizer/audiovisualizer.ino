#include "LedControl.h"

//Initialize the dot matrix display controller
//pin 12, DataIn
//pin 11, CLK
//pin 10, LOAD/CS
//We have 4 x MAX72XX.

LedControl dotMatrix = LedControl(12,11,10,4);

int sensorPin = 3;      //pin number to use the ADC
float EMA_A = 0.4;      //initialization of EMA alpha
int EMA_S = 0;          //initialization of EMA S
    
void setup() {
  // put your setup code here, to run once:
  for(int x=0;x<4;x++){
dotMatrix.shutdown(x, false);
dotMatrix.setIntensity(x, 8);
dotMatrix.clearDisplay(x);
}

EMA_S = analogRead(sensorPin);  //set EMA S for t=1

}

int lastMillis = 0;
void loop() {
  // put your main code here, to run repeatedly:

int a0 = analogRead(0);
int a1 = analogRead(1);
int a2 = analogRead(2)/128;
int a3 = analogRead(3)/128;

sample(a3, EMA_A, EMA_S);

int millisNow = millis();

if (millisNow - lastMillis < 50) return;

lastMillis = millisNow;
  
dotMatrix.setRow(0,0,0xff);
dotMatrix.setRow(1,2,0xff);
dotMatrix.clearDisplay(2);
dotMatrix.setRow(2,a2,0xff);
dotMatrix.clearDisplay(3);
dotMatrix.setRow(3,EMA_S,0xff);
}

//Exponential Moving Average
//https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
void sample(int sensorValue, float ema_a, int& ema_s){
  ema_s = (ema_a * sensorValue) + ((1-ema_a)*ema_s);    //run the EMA
}

    
