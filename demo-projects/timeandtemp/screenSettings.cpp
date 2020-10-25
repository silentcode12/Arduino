#include "arduino.h"
#include "context.h"
#include "screen.h"
#include "screenSettings.h"

#define MENU_ITEM_COUNT 6
#define MAX_MENU_ITEM_LENGTH 9

const char menuItems[MENU_ITEM_COUNT][MAX_MENU_ITEM_LENGTH] PROGMEM = {"Set Time", "Set Date", "CLK %shr" , "Temp %s", "ACC %s",  "Exit"};  //Stored in flash, read out using sprintf_P with %S

void ScreenSettings::ProcessUpdateAction(const Context* context)
{
  if (activeMenuItem == setExit)
    activeMenuItem = setTime;
  else
    activeMenuItem = activeMenuItem + 1;
}

void ScreenSettings::ProcessCommitAction(const Context* context)
{
   switch(activeMenuItem)
   {
    case setTime:  //time
      context->GotoTimeEditScreen();
      break;
    case setDate: //date
      context->GotoDateEditScreen();
      break;
    case setClock: //clk
      context->SetIs24Hour(!context->GetIs24Hour());
      break;
    case setUnits: //metric
      context->SetIsMetric(!context->GetIsMetric());
      break;
    case setAutoChannelChange: //Auto channel change
      context->SetIsAutoChannelChange(!context->GetIsAutoChannelChange());
      break;
    default: //exit
      context->GotoTimeScreen();
      break;
   }
}

void ScreenSettings::OnShow(const Context* context)
{
    activeMenuItem = setTime;
}

bool ScreenSettings::AllowAutoChannelChange()
{
  return false;
}

#define x0 4
#define y0 16
#define w 120
#define h 32
#define radius 5
#define color 1
#define ARRAY_SIZE 10

void ExpandFormatString(char* dest, const char* src, const char* additionalData)
{
    char fmt[ARRAY_SIZE];  
    strcpy(fmt, src);
    sprintf(dest, fmt, additionalData);
}

void ScreenSettings::Render(const Context* context)
{
  context->drawRoundRect(x0, y0, w, h, radius, color);
  
  char data[ARRAY_SIZE];
  sprintf_P(data, PSTR("%S"), menuItems[activeMenuItem]);

  if (activeMenuItem == setClock)
  {
    //data contains a format specification, use it to add dynamic data
    ExpandFormatString(data, data, context->GetIs24Hour() ? "24" : "12");
  }
  else if (activeMenuItem == setUnits)
  {
    ExpandFormatString(data, data, context->GetIsMetric() ? "C" : "F");
  }
  else if (activeMenuItem == setAutoChannelChange)
  {
    ExpandFormatString(data, data, context->GetIsAutoChannelChange() ? "on" : "off");
  }
  
  context->drawText(data, 2, 64, 32, center, false);
}
