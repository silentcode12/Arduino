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

Context::Context(const RTC_DS3231* rtc, const BME280* bme280, const Adafruit_SSD1306* display, const void (*playAnimationCallback)())
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

DateTime Context::GetDateTime()
{
  return dateTime;
}

void Context::SetDateTime(DateTime& newDateTime)
{
  rtc->adjust(newDateTime);
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
  return percentRH.s;
}

float Context::GetTemperature()
{
  return temperature.s;
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
