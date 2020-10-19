#include "commonTypes.h"
#include <EEPROM.h>
#include <RTClib.h>
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <SparkFunBME280.h>
#include "screen.h"
#include "ema.h"
#include "context.h"
#include "context2.h"

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

Context2::Context2(const RTC_DS3231* rtc, const BME280* bme280, const Adafruit_SSD1306* display, const void (*playAnimationCallback)()) : 
percentRH (0.1, 0),
temperature (0.9, 0)
{
  this->rtc = rtc;
  this->bme280 = bme280;
  currentScreen = new ScreenTime();
  this->display = display;
  this->playAnimationCallback = playAnimationCallback;
}

void Context2::Begin()
{
  dateTime = rtc->now();
  //Prime the first value to avoid initial homing of average from zero.
  percentRH.Reset(bme280->readFloatHumidity());
  temperature.Reset(bme280->readTempF() + TEMP_CORRECTION);
}

Context2::~Context2()
{
  delete currentScreen;
}

byte backoff = 0;

void Context2::RefreshData()
{
  dateTime = rtc->now();
  temperature.AddSample(bme280->readTempF() + TEMP_CORRECTION);
  percentRH.AddSample(bme280->readFloatHumidity());

  if (backoff > 0)
    backoff--;
    
  if (backoff <= 0 && GetIsAutoChannelChange() && currentScreen->AllowAutoChannelChange() && dateTime.second() % 20 == 0)
  {
    currentScreen->ProcessUpdateAction(this);
  }

  currentScreen->Tick();
}

void Context2::GetDate(short& year, byte& month, byte& day)
{
  year = dateTime.year();
  month = (byte)dateTime.month();
  day = (byte)dateTime.day();
}

void Context2::SetDate(const short& year, const byte& month, const byte& day)
{
  rtc->adjust(DateTime(year, month, day, dateTime.hour(), dateTime.minute(), dateTime.second()));
}

void Context2::GetTime(byte& hour, byte& minute, byte& second)
{
  hour = dateTime.hour();
  minute = dateTime.minute();
  second = dateTime.second();
}

void Context2::SetTime(const byte& hour, const byte& minute, const byte& second)
{
  rtc->adjust(DateTime(dateTime.year(), dateTime.month(), dateTime.day(), hour, minute, second));
}

const char* Context2::GetDayOfWeek()
{
  return daysOfTheWeek[dateTime.dayOfTheWeek()];
}

void Context2::GotoDateScreen()
{
  SwapScreen(new ScreenDate());
}

void Context2::GotoTimeScreen()
{
  SwapScreen(new ScreenTime());
}

void Context2::GotoTimeEditScreen()
{
  SwapScreen(new ScreenTimeEdit());
}

void Context2::GotoTempScreen()
{
  SwapScreen(new ScreenTemp());
}

void Context2::GotoRhScreen()
{
  SwapScreen(new ScreenRh());
}

void Context2::GotoDateEditScreen()
{
  SwapScreen(new ScreenDateEdit());
}

void Context2::GotoSettingsScreen()
{
  SwapScreen(new ScreenSettings());
}

void Context2::SwapScreen(const Screen* newScreen)
{
  Screen* tobeDeleted = currentScreen;
  currentScreen = newScreen;
  delete tobeDeleted;
  playAnimationCallback();
  currentScreen->OnShow(this);
}

void Context2::SetSetting(byte settingMask, bool value)
{
  byte s;
  EEPROM.get((int)&settings, s);
  if (value)
    s |= settingMask;
  else
    s &= ~settingMask;
  EEPROM.put((int)&settings, s);
}

bool Context2::GetSetting(byte settingMask)
{
  byte s;
  EEPROM.get((int)&settings, s);
  return (s & settingMask) == settingMask;
}

bool Context2::GetIs24Hour()
{ 
  return GetSetting(IS_24HR);
}

void Context2::SetIs24Hour(bool value)
{
  SetSetting(IS_24HR, value);
}

bool Context2::GetIsMetric()
{
  return GetSetting(IS_METRIC);
}

void Context2::SetIsMetric(bool value)
{
  SetSetting(IS_METRIC, value);
}

bool Context2::GetIsAutoChannelChange()
{
  return GetSetting(IS_ACC);
}

void Context2::SetIsAutoChannelChange(bool value)
{
  SetSetting(IS_ACC, value);
}

float Context2::GetPercentRh()
{
  return percentRH.GetValue();
}

float Context2::GetTemperatureF()
{
  return temperature.GetValue();
}

void Context2::UserInputUpdate()
{
  backoff = 60;  //User actively selected screen so disable ACC for a while...
  currentScreen->ProcessUpdateAction(this);
}

void Context2::UserInputCommit()
{
  currentScreen->ProcessCommitAction(this);
}

void Context2::RefreshDisplay()
{
  display->clearDisplay();
  currentScreen->Render(this);
  display->display();
}

//Use this function if the text pointer is pointing to program memory.
void Context2::drawText_P(const char* text, int textSize, int16_t &x, int16_t &y, ALIGN align, bool superscript) 
{
     char* buf = new char[strlen_P(text)+1];
     strcpy_P(buf, text);
     drawText(buf, textSize, x, y, align, superscript);
     delete[] buf;
}

void Context2::drawText(const char* text, int textSize, int16_t &x, int16_t &y, ALIGN align, bool superscript)
{
  int16_t x1 = 0, y1 = 0;
  uint16_t w = 0, h = 0;
  display->setCursor(0,0);
  display->setTextWrap(false);
  display->setTextColor(WHITE);
  display->setTextSize(textSize);
  display->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  
  switch(align)
  {
    case left:
    
    break;
    case center:
      x -= w / 2;
      y -= h / 2;
    break;
    
    case right:
      x -= w;
      y -= superscript ? 2 * h : h;
    break;
  }

  display->setCursor(x, y);
  display->print(text);
}

void Context2::drawLine(int16_t x, int16_t y, int16_t x1, int16_t y1, int16_t color)
{
  display->drawLine(x, y, x1, y1, color);
}

void Context2::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, int16_t color)
{
  display->drawRoundRect(x, y, w, h, radius, color);
}
