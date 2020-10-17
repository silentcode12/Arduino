#include "commonTypes.h"
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <SparkFunBME280.h>
#include "context.h"
//#include "ema.h"

//Constants
#define OLED_RESET 4
#define BUTTON_PIN 2
#define RTC_SQW_PIN 3
#define RTC_32K_PIN 5

//Variables
byte buttonState = 1;
bool isLongPress = false;
float last = 0;

//I2C devices
Adafruit_SSD1306 display(OLED_RESET);
RTC_DS3231 rtc;
BME280 bme280;

void playAnimation()
{
  //Prevent the update interupt from painting the current screen during the animation.
  noInterrupts();
  detachInterrupt(digitalPinToInterrupt(RTC_SQW_PIN));
  interrupts();

  //Draw random on/off 4 x 4 squares across screen 
  for (int vHoldPos = SSD1306_LCDHEIGHT; vHoldPos > 0; vHoldPos--)
  {  
    for (int x = 0; x < SSD1306_LCDWIDTH; x += 4)  // go across the screen left to right
    {
      for (int y = 0; y < SSD1306_LCDHEIGHT; y += 4) //go down the screen top to bottom
      {
        long color = random(0, 4) % 2;  // Generate random 0 or 1.  Apparently random(0, 1) doesn't work.
        display.fillRect(x, y, 4, 4, color);
      }
    }

    //Draw a horizontall bar that moves up the screen to simulate vertical hold
    display.fillRect(0, vHoldPos, SSD1306_LCDWIDTH, 15, 0);

    vHoldPos -= 4;
    
    display.display();  //Note:  on the nano, this operation is slow enough that no delay is needed
  }

  //Animation complete, allow normal screen updates.
  attachInterrupt(digitalPinToInterrupt(RTC_SQW_PIN), pin3ISR, RISING);
}

Context context(&rtc, &bme280, &display, &playAnimation);

Ema button(0.5, 1);

void setup () 
{
  //Setup input pins
  pinMode(RTC_SQW_PIN, INPUT);         //Square wave from rtc
  pinMode(BUTTON_PIN, INPUT_PULLUP);  //Button pin
  
  //Configure the RTC
  rtc.begin();
  rtc.writeSqwPinMode(Ds3231SqwPinMode::DS3231_SquareWave1Hz);

  //Configure the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  //Configure the BME2800
  bme280.settings.commInterface = I2C_MODE;
  bme280.settings.I2CAddress = 0x76;
  bme280.settings.runMode = 3; //Normal mode
  bme280.settings.tStandby = 0;
  bme280.settings.filter = 0;
  bme280.settings.tempOverSample = 1;
  bme280.settings.pressOverSample = 1;
  bme280.settings.humidOverSample = 1;
  bme280.begin();

  //Configure the context
  context.Begin();

  //Setup interrupts
  attachInterrupt(digitalPinToInterrupt(RTC_SQW_PIN), pin3ISR, RISING);
}

void pin3ISR()
{
  static boolean IsRunning = false;
  if(IsRunning) 
    return;
  
  IsRunning = true;
  
  interrupts(); //Enable interrupts so that I2C communication can work, equivalent to sei();
  
  context.RefreshData();
  context.RefreshDisplay();

  noInterrupts();
  IsRunning = false;
}

void UpdateImmediate()
{
  noInterrupts();
  pin3ISR();
  interrupts();
}

void loop () 
{
  button.AddSample(digitalRead(BUTTON_PIN));
  byte s = (byte)button.GetValue();

  if (s != 0 && s != 1)
  {
    //The button is transitioning between states
    return;
  }

  if (buttonState != s)
  {
    if (s == 1)
    {
      ButtonUp();
      UpdateImmediate();
      isLongPress = false;
    }
    else
    {
      last = millis();
    }

    buttonState = s;
  }

  if (buttonState != 1) //button is down
  {
      //Button is still being pressed, check for how long it has been pressed and trigger the long press begin
      float now = millis();
      if (now - last > 2000 && !isLongPress)
      {
        //Button has been pressed for greater than threshold and this is the first time it was detected
        isLongPress = true; 
        LongPressBegin();
        UpdateImmediate();
      }
  }
}

void ButtonUp()
{
  if (!isLongPress)
  {
    context.UpdateInput();
  }
}

void LongPressBegin()
{
   context.CommitInput();
}
