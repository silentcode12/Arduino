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

//EEPROM memory mappings
SETTINGS settings EEMEM = {false, false}; //todo:  Figure out how to initialize eeprom memory at flash time.  looks like a separate binary file...

Context::Context(const RTC_DS3231& rtc, const BME280& bme280, const Adafruit_SSD1306& display, const void (*playAnimationCallback)())
{
  this->rtc = &rtc;
  this->bme280 = &bme280;
  currentScreen = new ScreenTime();
  this->display = &display;
  this->playAnimationCallback = playAnimationCallback;
}

void Context::Begin()
{
  //Prime the first value to avoid initial homing of average from zero.
  percentRH.s = bme280->readFloatHumidity();
  temperature.s = bme280->readTempF() + TEMP_CORRECTION;
  altitude.s = bme280->readFloatAltitudeFeet();
  pressure.s = bme280->readFloatPressure();

  dateTime = rtc->now();
}

Context::~Context()
{
  delete currentScreen;
}

void Context::RefreshData()
{
  dateTime = rtc->now();
  UpdateEma(temperature, bme280->readTempF() + TEMP_CORRECTION);
  UpdateEma(percentRH, bme280->readFloatHumidity());
  UpdateEma(pressure, bme280->readFloatPressure());
  UpdateEma(altitude, bme280->readFloatAltitudeFeet());
}

DateTime Context::GetCurrentDateTime()
{
  return dateTime;
}

void Context::GotoDateScreen()
{
  delete currentScreen;
  currentScreen = new ScreenDate();
  playAnimationCallback();
  currentScreen->OnShow(this);
}

void Context::GotoTimeScreen()
{
  delete currentScreen;
  currentScreen = new ScreenTime();
  playAnimationCallback();
  currentScreen->OnShow(this);
}

void Context::GotoTimeEditScreen()
{
  delete currentScreen;
  currentScreen = new ScreenTimeEdit();
  playAnimationCallback();
  currentScreen->OnShow(this);
}

void Context::GotoTempScreen()
{
  delete currentScreen;
  currentScreen = new ScreenTemp();
  playAnimationCallback();
  currentScreen->OnShow(this);
}

void Context::GotoRhScreen()
{
  delete currentScreen;
  currentScreen = new ScreenRh();
  playAnimationCallback();
  currentScreen->OnShow(this);
}

void Context::GotoDateEditScreen()
{
  delete currentScreen;
  currentScreen = new ScreenDateEdit();
  playAnimationCallback();
  currentScreen->OnShow(this);
}

void Context::SetDateTime(DateTime& newDateTime)
{
  rtc->adjust(newDateTime);
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

EMA Context::GetPercentRh()
{
  return percentRH;
}

EMA Context::GetTemperature()
{
  return temperature;
}

bool Context::IsLeapYear(short year)
{
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ?
      true :
      false;
}
