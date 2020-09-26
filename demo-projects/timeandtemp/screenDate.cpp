#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include "context.h"
#include "screen.h"
#include "screenDate.h"

void ScreenDate::ProcessCommitAction(const Context* context)
{
 
}

void ScreenDate::ProcessUpdateAction(const Context* context)
{
 
}

void ScreenDate::Render(const Adafruit_SSD1306* display, const Context* context)
{
  display->clearDisplay();
  int x, y;
  x = y = 10;
  drawText_P(display, PSTR("Date"), 1, x, y, left, false);

  DateTime dateTime = context->GetCurrentDateTime();
  //display date
  char data2[17];
  sprintf_P(data2, PSTR("%S %02d/%02d\n%d"), daysOfTheWeek[dateTime.dayOfTheWeek()], dateTime.month() ,dateTime.day(), dateTime.year());
  x = 0;
  y = 32;
  drawText(display, data2, 2, x, y, left, false);
  
  display->display();
}
