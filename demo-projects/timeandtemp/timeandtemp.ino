#include "commonTypes.h"
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <SparkFunBME280.h>
#include "ema.h"
#include "context.h"
#include "context2.h"

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
Context2 context(&rtc, &bme280, &display);
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

void UpdateContext(bool isUserInput)
{
  static boolean IsRunning = false;
  if(IsRunning) 
    return;
  
  IsRunning = true;
  
  interrupts(); //Enable interrupts so that I2C communication can work, equivalent to sei();
  
  context.RefreshData(isUserInput);
  context.RefreshDisplay();

  noInterrupts();
  
  IsRunning = false;  
}

void pin3ISR()
{
  UpdateContext(false);
}

void UpdateImmediate()
{
  noInterrupts();  //replicate the ISR behavior
  UpdateContext(true);
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
      //The button is still being pressed, check  how long the button has been pressed.
      float now = millis();
      if (now - last > 2000 && !isLongPress)
      {
        //Button has been pressed longer than the threshold, this is the first time it was detected so trigger the long press begin
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
    context.UserInputUpdate();
  }
}

void LongPressBegin()
{
   context.UserInputCommit();
}
