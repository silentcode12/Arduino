#include "arduino.h"
#include "context.h"
#include "screen.h"
#include "screenDate.h"

void ScreenDate::ProcessUpdateAction(const Context* context)
{
  context->GotoTempScreen();
}

void ScreenDate::Render(const Context* context)
{
  short year;
  byte day, month, dayOfWeek;
  context->GetDate(year, month, day, dayOfWeek);
  
  //display date
  char data2[17];
  sprintf_P(data2, PSTR("%S %S\n%02d %d"), context->GetDayString_P(dayOfWeek), context->GetMonthString_P(month-1), day, year);

  context->drawText(data2, 3, 0, 16, left, false);
}
