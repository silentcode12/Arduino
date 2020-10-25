#include "arduino.h"
#include "context.h"
#include "screen.h"
#include "screenTimeEdit.h"

ScreenTimeEdit::ScreenTimeEdit()
{
  editState = editHour;
}

void ScreenTimeEdit::ProcessUpdateAction(const Context* context)
{
  switch(editState)
    {
      case editHour: 
        if (hour >= 23)
          hour = 1;
        else
          hour++;
        break;
      case editMinute: 
        if(minute >= 59)
          minute = 0;
        else
          minute++;
        break;
      case editSecond:
        second = 0; //Note:  reset seconds to zero for synchronization, will count up
        return;
    }
}

void ScreenTimeEdit::ProcessCommitAction(const Context* context)
{
  if (editState == editSecond)
  {
     context->SetTime(hour, minute, second);
     context->GotoTimeScreen();
  }
  else
  {
    editState = editState + 1;
  }
}

void ScreenTimeEdit::Render(const Context* context)
{
  int x, y;
  x = 10;
  y = 30;

  char data[10];
  if (context->GetIs24Hour())
    sprintf_P(data, PSTR("%02d:%02d:%02d"), hour, minute, second);
  else
    sprintf_P(data, PSTR("%02d:%02d:%02d %S"), hour > 12 ? hour - 12 : hour, minute, second, hour > 11 ? PSTR("P") : PSTR("A"));
    
  context->drawText(data, 2, x, y, left, false);

  //Draw a selection line under the active field.
  int w;

  w = 24;
  
  for(y=45; y <48; y ++)
  {
    x = 10 
    + (editState * w) //numeric digits width
    + (editState * w / 2);  //divider width
    int x1 = x + w;  // underline the two digits
    context->drawLine(x, y, x1, y, 1);
  }
}

void ScreenTimeEdit::OnShow(const Context* context)
{
  context->GetTime(hour, minute, second);
  second = 0;
  editState = editHour;
}

bool ScreenTimeEdit::AllowAutoChannelChange()
{
  return false;
}

void ScreenTimeEdit::Tick()
{
  if (editState == editSecond)
  {
    if (second + 1 > 59)
      second = 0;
    else
      second++;
  }
}
