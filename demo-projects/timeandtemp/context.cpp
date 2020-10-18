#include "commonTypes.h"
#include <EEPROM.h>
#include <RTClib.h>
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <SparkFunBME280.h>
#include "screen.h"
#include "ema.h"
#include "context.h"

#include "screenDate.h"
#include "screenDateEdit.h"
#include "screenTime.h"
#include "screenTimeEdit.h"
#include "screenTemp.h"
#include "screenRh.h"
#include "screenSettings.h"

#define IS_METRIC 0x1
#define IS_24HR 0x2
#define IS_ACC 0x4

//EEPROM memory mappings
byte settings EEMEM = {0}; //todo:  Figure out how to initialize eeprom memory at flash time.  looks like a separate binary file...

#define TEMP_CORRECTION -10.0

const char daysOfTheWeek[7][4] PROGMEM = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  //Stored in flash, read out using sprintf_P with %S

Context::Context(const RTC_DS3231* rtc, const BME280* bme280, const Adafruit_SSD1306* display, const void (*playAnimationCallback)()) : 
percentRH (0.1, 0),
temperature (0.9, 0)//,
//altitude (0.1, 0),
//pressure (0.1, 0)
{
  this->rtc = rtc;
  this->bme280 = bme280;
  currentScreen = new ScreenTime();
  this->display = display;
  this->playAnimationCallback = playAnimationCallback;
}

void Context::Begin()
{
  dateTime = rtc->now();
  //Prime the first value to avoid initial homing of average from zero.
  percentRH.Reset(bme280->readFloatHumidity());
  temperature.Reset(bme280->readTempF() + TEMP_CORRECTION);
  //altitude.Reset(bme280->readFloatAltitudeFeet());
  //pressure.Reset(bme280->readFloatPressure());
}

Context::~Context()
{
  delete currentScreen;
}

byte backoff = 0;

void Context::RefreshData()
{
  dateTime = rtc->now();
  temperature.AddSample(bme280->readTempF() + TEMP_CORRECTION);
  percentRH.AddSample(bme280->readFloatHumidity());
  //pressure.AddSample(bme280->readFloatPressure());
  //altitude.AddSample(bme280->readFloatAltitudeFeet());

  if (backoff > 0)
    backoff--;
    
  if (backoff <= 0 && IsAutoChannelChange() && currentScreen->AllowAutoChannelChange() && dateTime.second() % 20 == 0)
  {
    currentScreen->ProcessUpdateAction(this);
  }
}

void Context::GetDate(short& year, byte& month, byte& day)
{
  year = dateTime.year();
  month = (byte)dateTime.month();
  day = (byte)dateTime.day();
}

void Context::SetDate(const short& year, const byte& month, const byte& day)
{
  rtc->adjust(DateTime(year, month, day, dateTime.hour(), dateTime.minute(), dateTime.second()));
}

void Context::GetTime(byte& hour, byte& minute, byte& second)
{
  hour = dateTime.hour();
  minute = dateTime.minute();
  second = dateTime.second();
}

void Context::SetTime(const byte& hour, const byte& minute, const byte& second)
{
  rtc->adjust(DateTime(dateTime.year(), dateTime.month(), dateTime.day(), hour, minute, second));
}

const char* Context::GetDayOfWeek()
{
  return daysOfTheWeek[dateTime.dayOfTheWeek()];
}

void Context::GotoDateScreen()
{
  SwapScreen(new ScreenDate());
}

void Context::GotoTimeScreen()
{
  SwapScreen(new ScreenTime());
}

void Context::GotoTimeEditScreen()
{
  SwapScreen(new ScreenTimeEdit());
}

void Context::GotoTempScreen()
{
  SwapScreen(new ScreenTemp());
}

void Context::GotoRhScreen()
{
  SwapScreen(new ScreenRh());
}

void Context::GotoDateEditScreen()
{
  SwapScreen(new ScreenDateEdit());
}

void Context::GotoSettingsScreen()
{
  SwapScreen(new ScreenSettings());
}

void Context::SwapScreen(const Screen* newScreen)
{
  delete currentScreen;
  currentScreen = newScreen;
  playAnimationCallback();
  currentScreen->OnShow(this);
}

bool Context::Is24Hour()
{ 
  byte s;
  EEPROM.get((int)&settings, s);
  return (s & IS_24HR) == IS_24HR;
}

void Context::SetIs24Hour(bool value)
{
  byte s;
  EEPROM.get((int)&settings, s);
  if (value)
    s |= IS_24HR;
  else
    s &= ~IS_24HR;
  EEPROM.put((int)&settings, s);
}

bool Context::IsMetric()
{
  byte s;
  EEPROM.get((int)&settings, s);
  return (s & IS_METRIC) == IS_METRIC;
}

void Context::SetIsMetric(bool value)
{
  byte s;
  EEPROM.get((int)&settings, s);
  if (value)
    s |= IS_METRIC;
  else
    s &= ~IS_METRIC;
  EEPROM.put((int)&settings, s);
}

bool Context::IsAutoChannelChange()
{
  byte s;
  EEPROM.get((int)&settings, s);
  return (s & IS_ACC) == IS_ACC;
}

void Context::SetIsAutoChannelChange(bool value)
{
  byte s;
  EEPROM.get((int)&settings, s);
  if (value)
    s |= IS_ACC;
  else
    s &= ~IS_ACC;
  EEPROM.put((int)&settings, s);
}

float Context::GetPercentRh()
{
  return percentRH.GetValue();
}

float Context::GetTemperature()
{
  return temperature.GetValue();
}

void Context::UpdateInput()
{
  backoff = 60;  //User actively selected screen so disable ACC for a while...
	currentScreen->ProcessUpdateAction(this);
}

void Context::CommitInput()
{
	currentScreen->ProcessCommitAction(this);
}

void Context::RefreshDisplay()
{
  display->clearDisplay();
  currentScreen->Render(display, this);
  display->display();
}
