//This project is a fantasy security console that simulates a bio metric security scan.
//A strip of addressable rgb leds simulates the scanning process when triggered by a proximity sensor.
#include <PololuLedStrip.h>

#include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

const int sensorPin = 2;   
const int speakerPin = 7;  
const int frameCount = 5;
volatile int currentFrame = 0;
volatile bool doAnimate = true;  
volatile unsigned long sensorTriggeredMillis = 0;

PololuLedStrip<12> ledStrip;

#define LED_COUNT 5
rgb_color colors[LED_COUNT];

void setup() {
  pinMode(sensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sensorPin), SensorTriggered, FALLING );
  
  pinMode(speakerPin, OUTPUT);

  // Boot up sound
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(speakerPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(speakerPin);
  }
}

void loop() 
{
  unsigned long sensorTriggerDuration = millis() - sensorTriggeredMillis;

  rgb_color color;
  int state = digitalRead(sensorPin);

  if(state == LOW)
  {   
    //When the proximity sensor is trigger, simulate a biometric scan for 1 second
    if (sensorTriggerDuration < 1000)
    {
      WriteColor((rgb_color){0, 0, 50}, true);
      delay(50);
      return;
    }
    else 
    {
      //After one second set the led strip to solid green and play a sound.
      color = (rgb_color){0, 50, 0};

      if (doAnimate){
         tone(speakerPin, 5200, 500);
      }
         
      doAnimate = false;
    }
  }
  else
  {
    //While waiting to scan show a red pattern indicating access denied
    doAnimate = true;
    color = (rgb_color){50, 0, 0};
  }

  WriteColor(color, doAnimate);
        
  delay(100);
}

void SensorTriggered() {
    //Mark the time the proximity sensor was triggered
    sensorTriggeredMillis = millis();
}

//Write colors to the LED strip.  
//color:  The color to display
//animate:  Swipe an off led across the strip to make it more interesting
void WriteColor(rgb_color color, bool animate){
  for(uint16_t i = 0; i < LED_COUNT; i++)
  {
    colors[i] = color;
  }
  
  if (currentFrame++ >= frameCount)
      currentFrame=0;
  
  if (animate)
    colors[currentFrame] = (rgb_color){0,0,0};
  
  // Write to the LED strip.
  ledStrip.write(colors, LED_COUNT);  
}
