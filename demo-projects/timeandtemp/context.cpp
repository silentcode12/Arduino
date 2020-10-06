#include "commonTypes.h"
#include <EEPROM.h>
#include <RTClib.h>
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
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

Context::Context()
{
  currentScreen = new ScreenTime();
}

Context::~Context()
{
  delete currentScreen;
}

DateTime Context::GetCurrentDateTime()
{
}

void Context::GotoDateScreen()
{
  delete currentScreen;
  currentScreen = new ScreenDate();
  //playAnimation();
  currentScreen->OnShow(this);
}

void Context::GotoTimeScreen()
{
  delete currentScreen;
  currentScreen = new ScreenTime();
  //playAnimation();
  currentScreen->OnShow(this);
}

void Context::GotoTimeEditScreen()
{
  delete currentScreen;
  currentScreen = new ScreenTimeEdit();
  //playAnimation();
  currentScreen->OnShow(this);
}

void Context::GotoTempScreen()
{
  delete currentScreen;
  currentScreen = new ScreenTemp();
  //playAnimation();
  currentScreen->OnShow(this);
}

void Context::GotoRhScreen()
{
  delete currentScreen;
  currentScreen = new ScreenRh();
  //playAnimation();
  currentScreen->OnShow(this);
}

void Context::GotoDateEditScreen()
{
  delete currentScreen;
  currentScreen = new ScreenDateEdit();
  //playAnimation();
  currentScreen->OnShow(this);
}

void Context::SetDateTime(DateTime& newDateTime)
{
  //rtc.adjust(newDateTime);
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
  
}

EMA Context::GetTemperature()
{
}

bool Context::IsLeapYear(short year)
{
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ?
      true :
      false;
}
