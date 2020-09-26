#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include "context.h"
#include "screen.h"
#include "screenDateEdit.h"

ScreenDateEdit::ScreenDateEdit()
{
  dateIndex = 0;
}

void ScreenDateEdit::ProcessCommitAction(const Context* context)
{
 
}

void ScreenDateEdit::ProcessUpdateAction(const Context* context)
{
 
}

void ScreenDateEdit::Render(const Adafruit_SSD1306* display, const Context* context)
{
  display->clearDisplay();
  
  int x, y;
  x = y = 10;
  drawText_P(display, PSTR("Edit date"), 1, x, y, left, false);
  
  int year = date[0];
  byte month = date[1];
  byte day = date[2];

  y += 10;
  char data[6];
  if(dateIndex <= 3)
  {
    //set year
    drawText_P(display, PSTR("Edit year"), 1, x, y, left, false);
    y += 10;

    sprintf_P(data, PSTR("%d"), year);

    drawText(display, data, 1, x, y, left, false);
    int w = 4;
    for(y=45; y <48; y ++)
    {
      x = 10;
      x = x 
      + (dateIndex * w) //numeric digits width
      + (dateIndex * w / 2);  //divider width
      int x1 = x + w;  // underline the two digits
      display->drawLine(x, y, x1, y, 1);
    }
  }
  else if (dateIndex == 4)
  {
    //set month
    drawText_P(display, PSTR("Edit month"), 1, x, y, left, false);
    y += 10;

     sprintf_P(data, PSTR("%d"), month);

    drawText(display, data, 1, x, y, left, false);
  }
  else if (dateIndex == 5)
  {
    //set day
    drawText_P(display, PSTR("Edit day"), 1, x, y, left, false);
    y += 10;

    sprintf_P(data, PSTR("%d"), day);

    drawText(display, data, 1, x, y, left, false);
  }

  display->display();
}
