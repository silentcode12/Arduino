#include "arduino.h"
#include "context.h"
#include "screen.h"
#include "screenDate.h"

const char monthOfYear[12][4] PROGMEM = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};  //Stored in flash, read out using sprintf_P with %S

void ScreenDate::ProcessUpdateAction(const Context* context)
{
  context->GotoTempScreen();
}

void ScreenDate::Render(const Context* context)
{
  short year;
  byte day, month;
  context->GetDate(year, month, day);
  
  //display date
  char data2[17];
  sprintf_P(data2, PSTR("%S %S\n%d %d"), context->GetDayOfWeek(), monthOfYear[month-1], day, year);

  context->drawText(data2, 3, 0, 16, left, false);
}
