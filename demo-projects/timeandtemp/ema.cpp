#include "ema.h"

//Exponential Moving Average
//https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
Ema::Ema(float alpha, float initialValue)
{
  a = alpha;
  s = initialValue;  
}

void Ema::AddSample(float sample)
{
  s = (a * sample) + ((1-a)*s);    //run the EMA
}

void Ema::Reset(float value)
{
  s = value;
}

float Ema::GetValue(){
  return s;
}
