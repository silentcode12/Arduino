/* LedStripRainbow: Example Arduino sketch that shows
 * how to make a moving rainbow pattern on an
 * Addressable RGB LED Strip from Pololu.
 *
 * To use this, you will need to plug an Addressable RGB LED
 * strip from Pololu into pin 12.  After uploading the sketch,
 * you should see a moving rainbow.
 */

#include <PololuLedStrip.h>

// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<2> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 24
rgb_color colors[LED_COUNT];

int buttonState1;             // the current reading from the input pin
int lastButtonState1 = LOW;   

int buttonState2;             // the current reading from the input pin
int lastButtonState2 = LOW;   

unsigned long lastDebounceTime1 = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

unsigned long lastDebounceTime2 = 0;  // the last time the output pin was toggled

typedef void (*vvFunction)();

int DELAY = 10;
int flameValue = 0;
uint16_t solidDegrees = 0;

vvFunction callThis;

int mode;

void setup()
{
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);

  mode = 0;
  callThis = rainbow;
}

// Converts a color from HSV to RGB.
// h is hue, as a number between 0 and 360.
// s is the saturation, as a number between 0 and 255.
// v is the value, as a number between 0 and 255.
rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return (rgb_color){r, g, b};
}



void setMode()
{
  mode+= 1;
  if (mode > 4)
    mode = 0;

    switch(mode)
    {
       case 0: 
           callThis = rainbow; 
           break;
       case 1: 
           callThis = flame; 
           break;
       case 2: 
           callThis = solid; 
           break;
       case 3: 
           callThis = fade; 
           break;
       default: 
           mode = 0;
           callThis = rainbow;
           break;
    }
}

void setDelay()
{
  switch(mode)
    {
      case 1: 
      {
         flameValue += 25;
         if (flameValue > 128)
             flameValue = 0;
         DELAY = 10; 
         
         break;  
      }
      case 2:
      {         
         solidDegrees += 17;
         if (solidDegrees > 360)
            solidDegrees = 0;
         DELAY = 10;
            
         break;
      }      
      default:
      {
        DELAY += 10;
        if (DELAY > 100)
          DELAY = 1;
      }        
   }
}

volatile unsigned long lastTime = 0;

void loop()
{
   // void debounce(int buttonPin, int &buttonState, int &lastButtonState, unsigned long &lastDebounceTime, vvFunction callOnChange)
    debounce(3, buttonState1, lastButtonState1, lastDebounceTime1, setDelay);
    debounce(4, buttonState2, lastButtonState2, lastDebounceTime2, setMode);

    unsigned long time = millis();
    if (time - lastTime > DELAY)
    {
        lastTime = time;
        callThis();
    }     
}

void flame()
{
  for(uint16_t i = 0; i < LED_COUNT; i++)
  {
    colors[i] = hsvToRgb(random(15, 25), 255, random(64, 128) + flameValue);
  }

  ledStrip.write(colors, LED_COUNT);   
}

void solid()
{
  for(uint16_t i = 0; i < LED_COUNT; i++)
  {
    colors[i] = hsvToRgb(solidDegrees, 255, 128);
  }

  ledStrip.write(colors, LED_COUNT); 
}

int colorDegrees = 0;
int colorValue = 0;
int colorDirection = 1;

void fade()
{
  colorValue += (1 * colorDirection);
  
  if (colorValue >= 254)
  {
      colorDirection = -1;
  }
  else if (colorValue <= 1)
  {
      if (colorDegrees >= 360)
          colorDegrees = 0;
      else
          colorDegrees += 10;

      colorDirection = 1;
  }
  
  for(uint16_t i = 0; i < LED_COUNT; i++)
  {
    colors[i] = hsvToRgb(colorDegrees, 255, colorValue);
  }

  ledStrip.write(colors, LED_COUNT); 
}

void rainbow()
{
  uint16_t time = millis() >> 2;
  for(uint16_t i = 0; i < LED_COUNT; i++)
  {
    byte x = (time >> 2) - (i << 3);
    colors[i] = hsvToRgb((uint32_t)x * 359 / 256, 255, 128);
  }

  ledStrip.write(colors, LED_COUNT); 
}

void debounce(int buttonPin, int &buttonState, int &lastButtonState, unsigned long &lastDebounceTime, vvFunction callOnChange)
{
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        callOnChange();
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}
