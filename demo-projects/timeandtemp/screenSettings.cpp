#include "commonTypes.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <SparkFunBME280.h>
#include <RTClib.h>
#include "context.h"
#include "screen.h"
#include "screenSettings.h"


const char menuItems[4][9] PROGMEM = {"Set Time", "Set Date", "Exit"};  //Stored in flash, read out using sprintf_P with %S

void ScreenSettings::ProcessCommitAction(const Context* context)
{
   switch(current)
   {
    case 0:
      context->GotoTimeEditScreen();
      break;
    case 1:
      context->GotoDateEditScreen();
      break;
    case 2:
      context->GotoTimeScreen();
      break;
   }
}

void ScreenSettings::ProcessUpdateAction(const Context* context)
{
    current++;
    if (current >= count)
      current = 0;
}

void ScreenSettings::OnShow(const Context* context)
{
    current = 0;
}

#define x0 4
#define y0 16
#define w 120
#define h 32
#define radius 5
#define color 1

void ScreenSettings::Render(const Adafruit_SSD1306* display, const Context* context)
{
  display->drawRoundRect(x0, y0, w, h, radius, color);
  
  char data2[9];
  sprintf_P(data2, PSTR("%S"), menuItems[current]);
  int x = 64;
  int y = 32;
  drawText(display, data2, 2, x, y, center, false);
}
