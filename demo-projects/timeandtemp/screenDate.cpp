#include "commonTypes.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <SparkFunBME280.h>
#include "ema.h"
#include "context.h"
#include "screen.h"
#include "screenDate.h"

const char monthOfYear[12][4] PROGMEM = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Nov", "Dec"};  //Stored in flash, read out using sprintf_P with %S

void ScreenDate::ProcessUpdateAction(const Context* context)
{
  context->GotoTempScreen();
}

void ScreenDate::Render(const Adafruit_SSD1306* display, const Context* context)
{
  int x, y;
  x = y = 10;

  short year;
  byte day, month;
  context->GetDate(year, month, day);
  
  //display date
  char data2[17];
  sprintf_P(data2, PSTR("%S %S\n%d %d"), context->GetDayOfWeek(), monthOfYear[month-1], day, year);
  x = 0;
  y = 16;
  drawText(display, data2, 3, x, y, left, false);
}
