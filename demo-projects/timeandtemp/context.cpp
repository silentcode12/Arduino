#include "commonTypes.h"
#include <EEPROM.h>
#include <RTClib.h>
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <SparkFunBME280.h>
#include "screen.h"
#include "context.h"

#include "screenDate.h"
#include "screenDateEdit.h"
#include "screenTime.h"
#include "screenTimeEdit.h"
#include "screenTemp.h"
#include "screenRh.h"
#include "screenSettings.h"

typedef struct _SETTINGS
{
  bool is24hr;
  bool isMetric;
}SETTINGS;

//EEPROM memory mappings
SETTINGS settings EEMEM = {false, false}; //todo:  Figure out how to initialize eeprom memory at flash time.  looks like a separate binary file...

#define TEMP_CORRECTION -10.0

const char daysOfTheWeek[7][4] PROGMEM = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  //Stored in flash, read out using sprintf_P with %S

Context::Context(const RTC_DS3231* rtc, const BME280* bme280, const Adafruit_SSD1306* display, const void (*playAnimationCallback)()) : 
percentRH (0.1, 0),
temperature (0.9, 0),
altitude (0.1, 0),
pressure (0.1, 0)
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
  altitude.Reset(bme280->readFloatAltitudeFeet());
  pressure.Reset(bme280->readFloatPressure());
}

Context::~Context()
{
  delete currentScreen;
}

void Context::RefreshData()
{
  dateTime = rtc->now();
  temperature.AddSample(bme280->readTempF() + TEMP_CORRECTION);
  percentRH.AddSample(bme280->readFloatHumidity());
  pressure.AddSample(bme280->readFloatPressure());
  altitude.AddSample(bme280->readFloatAltitudeFeet());
}

void Context::GetDate(short& year, short& month, short& day)
{
  year = dateTime.year();
  month = dateTime.month();
  day = dateTime.day();
}

void Context::SetDate(const short& year, const short& month, const short& day)
{
  rtc->adjust(DateTime(year, month, day, dateTime.hour(), dateTime.minute(), dateTime.second()));
}

void Context::GetTime(short& hour, short& minute, short& second)
{
  hour = dateTime.hour();
  minute = dateTime.minute();
  second = dateTime.second();
}

void Context::SetTime(const short& hour, const short& minute, const short& second)
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
  SETTINGS s;
  EEPROM.get((int)&settings, s);
  if (s.is24hr != 0 && s.is24hr != 1)
  {
    return true;
  }
  
  return s.is24hr;
}

void Context::SetIs24Hour(bool value)
{
  SETTINGS s;
  EEPROM.get((int)&settings, s);
  s.is24hr = value;
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
