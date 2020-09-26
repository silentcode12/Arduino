#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include "context.h"
#include "screen.h"
#include "screenTimeEdit.h"

ScreenTimeEdit::ScreenTimeEdit()
{
  timeIndex = 0;
}

void ScreenTimeEdit::ProcessCommitAction(const Context* context)
{
 
}

void ScreenTimeEdit::ProcessUpdateAction(const Context* context)
{
 
}

void ScreenTimeEdit::Render(const Adafruit_SSD1306* display, const Context* context)
{
  display->clearDisplay();
  int x, y;
  x = y = 10;
  drawText_P(display, PSTR("Edit time"), 1, x, y, left, false);
  y += 20;

  if (timeIndex == -1)
  {
    //Display the clock mode (12hr/24hr) setting first
    x = 64;
    y = 32;
    drawText_P(display, context->GetSettings().is24hr ? PSTR("24hr") : PSTR("12hr"), 2, x, y, center, false);
  }
  else
  {
    char data[10];
    if (context->GetSettings().is24hr)
      sprintf_P(data, PSTR("%02d:%02d:%02d"), time[0], time[1], time[2]);
    else
      sprintf_P(data, PSTR("%02d:%02d:%02d %S"), time[0] > 12 ? time[0] - 12 : time[0], time[1], time[2], time[0] > 11 ? PSTR("P") : PSTR("A"));
      
    drawText(display, data, 2, x, y, left, false);

    //Draw a selection line under the active field.  Use to two digit string for width.
    int w, h;
    //display.setTextSize(1);
   // display.getTextBounds(PSTR("00"), 0, 0, &x, &y, &w, &h);
    w = 24;
    y = 45;
  
    for(y=45; y <48; y ++)
    {
      x = 10 
      + (timeIndex * w) //numeric digits width
      + (timeIndex * w / 2);  //divider width
      int x1 = x + w;  // underline the two digits
      display->drawLine(x, y, x1, y, 1);
    }
  }
  
  display->display();
}
