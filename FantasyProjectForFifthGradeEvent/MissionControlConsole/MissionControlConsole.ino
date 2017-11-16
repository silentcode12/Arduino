//This is a simple movie prop project to simulate a fantasy NASA style console with fun light displays and simple user inputs.
//This project utilizes four dip switches, an 8x8 dotmatrix display, an eight digit segment display and a strip of rgb addressable leds;
//The eight digit segment display is a simple game of reach the target value.  One dip switch controls the direction and the other the speed of ascent or descent.
//The dot matrix display shows a scrolling bar graph of random height, a dip switch controls the speed (fast/slow).
//The addressable strip of 5 LEDs displays one of two random patterns, the remaining dip switch changes the pattern.
#include <PololuLedStrip.h>
#include "LedControl.h"

#define pin2 2
#define pin3 3
#define pin4 4
#define pin5 5
#define LED_COUNT 5
//A strip of 5 addressable LEDs
rgb_color colors[LED_COUNT];

/*
 Initialize the dot matrix display controller
 pin 10, DataIn
 pin 12, CLK
 pin 11, LOAD/CS
 We have only a single MAX72XX.
 */
LedControl dotMatrix = LedControl(10,12,11,1);

/*
 Initialize the segment display controller
 pin 7, DataIn
 pin 9, CLK
 pin 8, LOAD/CS
 We have only a single MAX72XX.
 */
LedControl segmentDisplay=LedControl(7, 9, 8, 1);

//Initialize the led strip controller
PololuLedStrip<13> ledStrip;

void setup() {
  //dip switch inputs
  pinMode(pin2, INPUT_PULLUP);
  pinMode(pin3, INPUT_PULLUP);
  pinMode(pin4, INPUT_PULLUP);
  pinMode(pin5, INPUT_PULLUP);

  //Initialize the displays
  dotMatrix.shutdown(0, false);
  segmentDisplay.shutdown(0, false);
  
  //Set the brightness
  dotMatrix.setIntensity(0, 8);
  segmentDisplay.setIntensity(0, 8);
  
  //Clear the display
  dotMatrix.clearDisplay(0);
  segmentDisplay.clearDisplay(0);
}

unsigned long millisNow = 0;

void loop() {
  // Read the switches to get state information
  int state1 = digitalRead(pin2);
  int state2 = digitalRead(pin3);
  int state3 = digitalRead(pin4);
  int state4 = digitalRead(pin5);

  // Read current time for all operations
  millisNow = millis();

  writeStrip(state1, 250);
  writeDotMatrix(state2 == HIGH ? 250 : 100);
  writeSegments(state3, state4 == HIGH ? 10: 100);
}

unsigned long lastWriteStripWrite = 0;
//mode:  Either high or low and controls the pattern to display
//delayTime:  Determines how often the pattern should move to next value
void writeStrip(int mode, int delayTime){
  if ((millisNow - lastWriteStripWrite) < delayTime) return;

  lastWriteStripWrite = millisNow;
  
  if (mode == HIGH){
    for (int x = 0; x < LED_COUNT; x++){
      colors[x] = (rgb_color){0, 25, 0};
    }
    
    colors[random(0, 9)] = (rgb_color){25, 0, 0};
  }
  else{
    for (int x = 0; x < LED_COUNT; x++){
      colors[x] = (rgb_color){25, 0, 0};
    }

    colors[random(0, 9)] = (rgb_color){0, 25, 0};
    colors[random(0, 9)] = (rgb_color){25, 25, 0};
  }

  ledStrip.write(colors, LED_COUNT);  
}

byte patterns[9]={B00000000, B00000001, B00000011, B00000111, B00001111, B00011111, B00111111, B01111111, B11111111};
byte samples[8];
unsigned long lastDotMatrixWrite = 0;
//delayTime:  Controls the refresh time of the next "sample"
void writeDotMatrix(int delayTime){
  if ((millisNow - lastDotMatrixWrite) < delayTime) return;

  lastDotMatrixWrite = millisNow;

  byte val = random(1, 9);
  for(int x = 0; x < 7; x++){//shift sample array
    samples[x] = samples[x+1];
  }
  
  samples[7]=val; //add new sample

  //write out the samples
  for (int x = 0; x < 8; x++){
    dotMatrix.setRow(0,x,patterns[samples[x]]);
  }
}

unsigned long lastSegmentWrite = 0;
int num = 0;
int targetNum = 300;
int delayExtension = 0;
int delta = 1;
int lastMode = HIGH;
//mode:  Indicator to change direction
//delayTime:  refresh time of display
void writeSegments(int mode, int delayTime){
    UpdateDirectionIndicator(delta);
    
    if ((millisNow - lastSegmentWrite) < delayTime + delayExtension) return;

    lastSegmentWrite = millisNow;

    //change direction if mode changed.
    if (mode != lastMode){
      delta *= -1;
      lastMode = mode;
      }

    num += delta;

    //check upper lower boundary and reverse direction.
    if (num < -999) {
      num = -998;
      delta = 1;
    }
    else if (num > 999) {
      num = 998;
      delta = -1;
    }

    printNumber(num, false);
    printNumber(targetNum, true);

    if (num == targetNum){
      targetNum = random(-999, 1000);
      delayExtension = 1000; //pause on target reached
    }
    else{
      delayExtension = GetDelayExtension(targetNum, num);
    }
}

unsigned long directionIndicatorOnUpdate = 0;
bool directionIndicatorOn = true;
//Display a flashing ascending/descending indicator.
//delta:  Current direction of the target number
void UpdateDirectionIndicator(int delta){
  const byte tp = B01000000; //Use top segment for ascending indicator
  const byte ng = B00000001; //Use middle segment for negitive number indicator.
  const byte bt = B00001000; //Use bottom segment for descending indicator
  byte dirAndNeg = num < 0 ? ng : 0;

  //Blink the direction indicator on/off
  if ((millisNow - directionIndicatorOnUpdate) > 500) 
  {
      directionIndicatorOnUpdate = millisNow;
      directionIndicatorOn = !directionIndicatorOn;
  }

  if (directionIndicatorOn)
  {
    if (delta < 0)
    {
      dirAndNeg |= bt;
    }
    else
    {
      dirAndNeg |= tp;
    }
  }

  //Use setRow to set the individual segment elements
  segmentDisplay.setRow(0, 3, dirAndNeg);
}

//Slow the refresh rate as number approaches target.
int GetDelayExtension(int current, int target)
{
  //log10 delay extension pattern as current number approaches target number
  int distance = abs(target - current);
  if (distance > 100) return 0;
  if (distance > 90) return  30;
  if (distance > 80) return  48;
  if (distance > 70) return  60;
  if (distance > 60) return  70;
  if (distance > 50) return  78;
  if (distance > 40) return  85;
  if (distance > 30) return  90;
  if (distance > 20) return  95;
  if (distance > 10) return  100;
}

void printNumber(int value, bool secondDisplay) {  
    int digitOffset = secondDisplay ? 4 : 0;

    boolean negative=false;

    if(value < -999 || value > 999)  
        return;
          
    if(value < 0 ) {  
        negative = true; 
        value = value * -1;  
    }
    
    int ones = value % 10;  
    int tens= (value / 10) % 10;  
    int hundreds = (value / 100) % 10;  
    
    //print character '-' in the leftmost column when negative
    segmentDisplay.setChar(0, 3 + digitOffset, negative ? '-' : ' ', false); 
    
    //Now print the number digit by digit 
    segmentDisplay.setDigit(0, 2 + digitOffset,(byte)hundreds,false);
    segmentDisplay.setDigit(0, 1 + digitOffset,(byte)tens,false); 
    segmentDisplay.setDigit(0, 0 + digitOffset,(byte)ones,false); 
} 

