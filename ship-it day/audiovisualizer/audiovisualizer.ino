#include "LedControl.h"

int scaler = 64;

//Initialize the dot matrix display controller
//pin 12, DataIn
//pin 11, CLK
//pin 10, LOAD/CS
//We have 4 x MAX72XX.

LedControl dotMatrix = LedControl(12,11,10,4);

const byte interruptPin = 2;

typedef struct 
{
  float a;      //initialization of EMA alpha
  long s;        //initialization of EMA S
} EMA;

typedef struct
{
  EMA low;
  EMA high;
  long bandpass;
} EMA_BAND;

EMA_BAND ema_leftLow = { (EMA){.1, 0}, (EMA){.3,0} };
EMA_BAND ema_leftMid = { (EMA){.4, 0}, (EMA){.6,0} };
EMA_BAND ema_leftHigh = { (EMA){.7, 0}, (EMA){.9,0} };
EMA ema_leftVu = {.2, 0};

EMA_BAND ema_rightLow = { (EMA){.1, 0}, (EMA){.3,0} };
EMA_BAND ema_rightMid = { (EMA){.4, 0}, (EMA){.6,0} };
EMA_BAND ema_rightHigh = { (EMA){.7, 0}, (EMA){.9,0} };
EMA ema_rightVu = {.2, 0};
    
void setup() 
{
  // put your setup code here, to run once:
  for(int x=0;x<4;x++)
  {
    dotMatrix.shutdown(x, false);
    dotMatrix.setIntensity(x, 8);
    dotMatrix.clearDisplay(x);
  }
  
  ema_leftVu.s = ema_leftLow.low.s = ema_leftLow.high.s = ema_leftMid.low.s = ema_leftMid.high.s = ema_leftHigh.low.s = ema_leftHigh.high.s = analogRead(0)/scaler;
  ema_rightVu.s = ema_rightLow.low.s = ema_rightLow.high.s = ema_rightMid.low.s = ema_rightMid.high.s = ema_rightHigh.low.s = ema_rightHigh.high.s = analogRead(1)/scaler;
  
  analogReference(INTERNAL);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), modeChange, CHANGE);
}

volatile bool mode = false;

void modeChange(){
  mode = !mode;

  if (mode)
  {
    scaler = 16;
    ema_leftVu.a = ema_rightVu.a = .4;
  }
  else
  {
    scaler = 64;
    ema_leftVu.a = ema_rightVu.a = .25;
  }  
}

volatile long lastMillis = 0;

void loop() {
  long millisNow = millis();
    
  int a0 = analogRead(0)/scaler;
  ema(a0, ema_leftVu);
  bandPass(a0, ema_leftLow);
  bandPass(a0, ema_leftMid);
  bandPass(a0, ema_leftHigh);        

  int a1 = analogRead(1)/scaler;
  ema(a1, ema_rightVu);
  bandPass(a1, ema_rightLow);
  bandPass(a1, ema_rightMid);
  bandPass(a1, ema_rightHigh); 
  //~800uS for each sample
  //~1.25KHz sampling

  if (millisNow - lastMillis > 50) 
  {
    lastMillis = millisNow;

    if (scaler > 16)
    {
      drawvBar(3, ema_leftVu.s, ema_rightVu.s);
      drawvBar(0, ema_leftLow.bandpass, ema_rightLow.bandpass);
      drawvBar(1, ema_leftMid.bandpass, ema_rightMid.bandpass);
      drawvBar(2, ema_leftHigh.bandpass, ema_rightHigh.bandpass);
    }
    
    if (scaler <= 16)
    {
      drawhBar(ema_leftVu.s, ema_rightVu.s);
    }
  }
}

//Exponential Moving Average
//https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
void ema(int sensorValue, EMA& ema){
  ema.s = (ema.a * sensorValue) + ((1-ema.a)*ema.s);    //run the EMA
}

void bandPass(int sensorValue, EMA_BAND& bandEma){
  ema(sensorValue, bandEma.low);
  ema(sensorValue, bandEma.high);
   
  int highpass = sensorValue - bandEma.low.s;     //find the high-pass as before (for comparison)
  bandEma.bandpass = bandEma.high.s - bandEma.low.s;      //find the band-pass  
}

void drawvBar(int addr, int left, int right){
  int row = 8;
  
  while (--row >= 0)
  {
    int v = 0;
    
    if (left >= row)
    {
      v |= 0xe0;
    }

    if (right >= row)
    {
      v |= 0x0e;
    }

    dotMatrix.setRow(addr, row, v);
  }
}

void drawhBar(int left, int right)
{    
  for (int addr = 0; addr < 4; addr++)
  {  
    int column = 8;
    while (--column >= 0)
    {
      int v = 0;
      
      if (left >= column + (addr * 8))
      {
        v |= 0xe0;
      }
  
      if (right >= column + (addr * 8))
      {
        v |= 0x0e;
      }
  
      dotMatrix.setColumn(addr, 7 - column, v);    
    }
  }
}

    
