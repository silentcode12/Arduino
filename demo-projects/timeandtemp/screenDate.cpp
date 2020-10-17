#include "commonTypes.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <SparkFunBME280.h>
#include <RTClib.h>
#include "context.h"
#include "screen.h"
#include "screenDate.h"

void ScreenDate::ProcessUpdateAction(const Context* context)
{
  context->GotoTempScreen();
}

void ScreenDate::Render(const Adafruit_SSD1306* display, const Context* context)
{
  int x, y;
  x = y = 10;
  //drawText_P(display, PSTR("Date"), 1, x, y, left, false);

  DateTime dateTime = context->GetDateTime();
  //display date
  char data2[17];
  sprintf_P(data2, PSTR("%S %02d/%02d\n%d"), daysOfTheWeek[dateTime.dayOfTheWeek()], dateTime.month() ,dateTime.day(), dateTime.year());
  x = 0;
  y = 16;
  drawText(display, data2, 2, x, y, left, false);
}
