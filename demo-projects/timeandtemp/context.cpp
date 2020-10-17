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

//EEPROM memory mappings
SETTINGS settings EEMEM = {false, false}; //todo:  Figure out how to initialize eeprom memory at flash time.  looks like a separate binary file...

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
  //Prime the first value to avoid initial homing of average from zero.
  percentRH.Reset(bme280->readFloatHumidity());
  temperature.Reset(bme280->readTempF() + TEMP_CORRECTION);
  altitude.Reset(bme280->readFloatAltitudeFeet());
  pressure.Reset(bme280->readFloatPressure());

  dateTime = rtc->now();
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
  dateTime = rtc->now();
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
  dateTime = rtc->now();
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

SETTINGS Context::GetSettings()
{
  SETTINGS s;
  EEPROM.get((int)&settings, s);
  if (s.is24hr != 0 && s.is24hr != 1)
  {
    //EEPROM isn't initialized, do it now.
    s.is24hr = false;
    EEPROM.put((int)&settings, s);
  }
  
  return s;
}

void Context::SetSettings(SETTINGS newSettings)
{
  EEPROM.put((int)&settings, newSettings);
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
