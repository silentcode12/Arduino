#include "commonTypes.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include "context.h"
#include "screen.h"
#include "screenTime.h"

void ScreenTime::ProcessCommitAction(const Context* context)
{
  
}

void ScreenTime::ProcessUpdateAction(const Context* context)
{
   context->GotoDateScreen();
}

void ScreenTime::OnShow(const Context* context){}

void ScreenTime::Render(const Adafruit_SSD1306* display, const Context* context)
{
  display->clearDisplay();
  int x, y;
  x = y = 10;
  drawText_P(display, PSTR("Time"), 1, x, y, left, false); y += 20;
  char data[20];

  DateTime dateTime = context->GetCurrentDateTime();
  int hour = dateTime.hour();
  int minute = dateTime.minute();
  int second = dateTime.second();

  char tick = second % 2 == 1 ? ':' : ' ';
  
  if (context->GetSettings().is24hr)
    sprintf_P(data, PSTR("%02d%c%02d"), hour, tick, minute);
  else
    sprintf_P(data, PSTR("%02d%c%02d %s"), hour > 12 ? hour - 12 : hour, tick, minute, hour > 12 ? "P" : "A");

  x = 64;
  y = 40;
  drawText(display, data, 3, x, y, center, false);

  display->display();
}
