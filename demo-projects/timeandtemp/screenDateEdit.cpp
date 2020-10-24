#include "arduino.h"
#include "context.h"
#include "screen.h"
#include "screenDateEdit.h"

ScreenDateEdit::ScreenDateEdit()
{
  dateIndex = 0;
  year=2020;
}

void IncrementWithBounds(byte& value, byte max, byte resetValue)
{
  value++;
  if (value > max)
    value = resetValue;
}

void ScreenDateEdit::ProcessUpdateAction(const Context* context)
{
      //Deconstruct the year, Todo: Only do this when updated year parts
      byte thousands = (year % 10000) / 1000;
      byte hundreds = (year % 1000) / 100;
      byte tens = (year % 100) / 10;
      byte ones = year %10;
    
      switch (dateIndex)
      {
        case 0:
          IncrementWithBounds(thousands, 9, 1);
          break;  //update 1000s
        case 1:
          IncrementWithBounds(hundreds , 9, 0);
          break;//update 100s
        case 2:
          IncrementWithBounds(tens, 9, 0);
          break;//update 10s
        case 3:
          IncrementWithBounds(ones, 9, 0);
          break;//update 1s
        case 4:
          IncrementWithBounds(month, 12, 1);
          break;//update month
        case 5:
          byte max = 0;
          switch(month)
          {
            case 2:
              max = IsLeapYear(year) ? 29 : 28;
              break;
            case 4:
            case 6:
            case 9:
            case 11:
              max = 30;   //Apr4 Jun6 Sep9 Nov11
              break;
            default:
              max = 31; //Jan1 Mar3 May5 Jul7 Aug8 Oct10 Dec12
              break;
          }
          
          IncrementWithBounds(day, max, 1);
          break;//update day
      }

      //Reconstruct the year, Todo: Only do this when updated year parts
      year = thousands * 1000 + hundreds * 100 + tens * 10 + ones;
}

void ScreenDateEdit::ProcessCommitAction(const Context* context)
{
    dateIndex++;
    switch (dateIndex)
    {
      case 0: break;
      case 1: break;
      case 2: break;
      case 3: break;
      case 4: break;
      case 5: break;
      default: 
      {
        context->SetDate(year, month, day);
        context->GotoDateScreen();
        break;
      }
    }
}

void ScreenDateEdit::Render(const Context* context)
{
  int x, y;

  x = 0;
  y = 0;
  char data[6];
  if(dateIndex <= 3)
  {
    //set year
    context->drawText_P(PSTR("Year"), 1, x, y, left, false);

    //draw indicator column marker
    int w = 12;
    for(y=50; y <53; y ++)
    {
      x = 30;
      x = x 
      + (dateIndex * w) //numeric digits width
      + (dateIndex * w / 2);  //divider width
      int x1 = x + w;  // underline the two digits
      context->drawLine(x, y, x1, y, 1);
    }

    sprintf_P(data, PSTR("%d"), year);
  }
  else if (dateIndex == 4)
  {
    //set month
    context->drawText_P(PSTR("Month"), 1, x, y, left, false);

    sprintf_P(data, PSTR("%d"), month);
  }
  else if (dateIndex == 5)
  {
    //set day
    context->drawText_P(PSTR("Day"), 1, x, y, left, false);

    sprintf_P(data, PSTR("%d"), day);
  }

  context->drawText(data, 3, 64, 32, center, false);
}

void ScreenDateEdit::OnShow(const Context* context)
{
  context->GetDate(year, month, day);
  dateIndex = 0;
}

bool ScreenDateEdit::AllowAutoChannelChange()
{
  return false;
}

bool ScreenDateEdit::IsLeapYear(short year)
{
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ?
      true :
      false;
}
