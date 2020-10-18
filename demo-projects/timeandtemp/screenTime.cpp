#include "commonTypes.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include "context.h"
#include "screen.h"
#include "screenTime.h"

void ScreenTime::ProcessUpdateAction(const Context* context)
{
   context->GotoDateScreen();
}

void ScreenTime::Render(const Adafruit_SSD1306* display, const Context* context)
{
  int x, y;
  x = y = 10;

  char data[10];
  
  byte hour, minute, second;
  context->GetTime(hour, minute, second);

  char tick = second % 2 == 1 ? ':' : ' ';
  
  if (context->Is24Hour())
    sprintf_P(data, PSTR("%02d%c%02d"), hour, tick, minute);
  else
    sprintf_P(data, PSTR("%02d%c%02d %s"), hour > 12 ? hour - 12 : hour, tick, minute, hour > 12 ? "P" : "A");

  x = 64;
  y = 40;
  drawText(display, data, context->Is24Hour() ? 4 : 3, x, y, center, false);
}
