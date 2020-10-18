#include "commonTypes.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include "context.h"
#include "screen.h"
#include "screenTimeEdit.h"

ScreenTimeEdit::ScreenTimeEdit()
{
  timeIndex = 0;
}

void ScreenTimeEdit::OnShow(const Context* context)
{
  context->GetTime(hour, minute, second);
  second = 0;
  timeIndex = 0;
}

bool ScreenTimeEdit::AllowAutoChannelChange()
{
  return false;
}

void ScreenTimeEdit::Tick()
{
  if (timeIndex == 2)
  {
    if (second + 1 > 59)
      second = 0;
    else
      second++;
  }
}

void ScreenTimeEdit::ProcessCommitAction(const Context* context)
{
  timeIndex++;
  switch (timeIndex)
  {
    case 0: break;
    case 1: break;
    case 2: break;
    default: 
    {
      context->SetTime(hour, minute, second);
      context->GotoTimeScreen();
      break;
    }
  }
}

void ScreenTimeEdit::ProcessUpdateAction(const Context* context)
{
  switch(timeIndex)
    {
      case 0: 
        if (hour >= 23)
          hour = 0;

        hour++;
        break;
      case 1: 
        if(minute >= 59)
          minute = -1;

        minute++;
        break;
      case 2:
        second = 0; //Note:  reset seconds to zero for synchronization, will count up
        return;
    }
}

void ScreenTimeEdit::Render(const Adafruit_SSD1306* display, const Context* context)
{
  int x, y;
  x = y = 10;
 // drawText_P(display, PSTR("Edit time"), 1, x, y, left, false);
  y += 20;

  char data[10];
  if (context->Is24Hour())
    sprintf_P(data, PSTR("%02d:%02d:%02d"), hour, minute, second);
  else
    sprintf_P(data, PSTR("%02d:%02d:%02d %S"), hour > 12 ? hour - 12 : hour, minute, second, hour > 11 ? PSTR("P") : PSTR("A"));
    
  drawText(display, data, 2, x, y, left, false);

  //Draw a selection line under the active field.
  int w, h;

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
