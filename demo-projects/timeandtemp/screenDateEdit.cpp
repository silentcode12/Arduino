#include "arduino.h"
#include "context.h"
#include "screen.h"
#include "screenDateEdit.h"

ScreenDateEdit::ScreenDateEdit()
{
  editState = editYearThousands;
  year=2020;
}

void IncrementWithBounds(byte& value, byte max, byte resetValue)
{
  value++;
  if (value > max)
    value = resetValue;
}

byte ScreenDateEdit::GetMonthDays()
{
  switch(month)
  {
    case 2:
      return IsLeapYear() ? 29 : 28;  //Feb2
    case 4:
    case 6:
    case 9:
    case 11:
      return 30;   //Apr4 Jun6 Sep9 Nov11
    default:
      return 31; //Jan1 Mar3 May5 Jul7 Aug8 Oct10 Dec12
  }
}

void ScreenDateEdit::ProcessUpdateAction(const Context* context)
{
      //Deconstruct the year, Todo: Only do this when updated year parts
      byte thousands = (year % 10000) / 1000;
      byte hundreds = (year % 1000) / 100;
      byte tens = (year % 100) / 10;
      byte ones = year %10;
    
      switch (editState)
      {
        case editYearThousands:
          IncrementWithBounds(thousands, 9, 1);
          break;  //update 1000s
        case editYearHundreds:
          IncrementWithBounds(hundreds , 9, 0);
          break;//update 100s
        case editYearTens:
          IncrementWithBounds(tens, 9, 0);
          break;//update 10s
        case editYearOnes:
          IncrementWithBounds(ones, 9, 0);
          break;//update 1s
        case editMonth:
          IncrementWithBounds(month, 12, 1);
          break;//update month
        case editDay:         
          IncrementWithBounds(day, GetMonthDays(), 1);
          break;//update day
      }

      //Reconstruct the year, Todo: Only do this when updated year parts
      year = thousands * 1000 + hundreds * 100 + tens * 10 + ones;
}

void ScreenDateEdit::ProcessCommitAction(const Context* context)
{
  if (editState == editDay)
  {
    context->SetDate(year, month, day);
    context->GotoDateScreen();
  }
  else
  {
    editState = editState + 1;
    if (editState == editDay)
    {
      if (day > GetMonthDays())
      {
        //If previous day is outside of new month, reset to day one
        day = 1;
      }
    }
  }
}

void ScreenDateEdit::Render(const Context* context)
{
  int x, y;

  x = 0;
  y = 0;
  char data[6];
  if(editState <= editYearOnes)
  {
    //set year
    context->drawText_P(PSTR("Year"), 1, x, y, left, false);

    //draw indicator column marker
    int w = 12;
    for(y=50; y <53; y ++)
    {
      x = 30;
      x = x 
      + (editState * w) //numeric digits width
      + (editState * w / 2);  //divider width
      int x1 = x + w;  // underline the two digits
      context->drawLine(x, y, x1, y, 1);
    }

    sprintf_P(data, PSTR("%d"), year);
  }
  else if (editState == editMonth)
  {
    //set month
    context->drawText_P(PSTR("Month"), 1, x, y, left, false);

    sprintf_P(data, PSTR("%S"), context->GetMonthString_P(month - 1));
  }
  else if (editState == editDay)
  {
    //set day
    context->drawText_P(PSTR("Day"), 1, x, y, left, false);

    sprintf_P(data, PSTR("%d"), day);
  }

  context->drawText(data, 3, 64, 32, center, false);
}

void ScreenDateEdit::OnShow(const Context* context)
{
  byte dayOfWeek;
  context->GetDate(year, month, day, dayOfWeek);
  editState = editYearThousands;
}

bool ScreenDateEdit::AllowAutoChannelChange()
{
  return false;
}

bool ScreenDateEdit::IsLeapYear()
{
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ?
      true :
      false;
}
