#include "commonTypes.h"
#include "Adafruit_GFX.h"
#include "context.h"
#include "screen.h"
#include "screenSettings.h"

#define MENUITEMCOUNT 6

const char menuItems[MENUITEMCOUNT][9] PROGMEM = {"Set Time", "Set Date", "CLK %shr" , "Temp %s", "ACC %s",  "Exit"};  //Stored in flash, read out using sprintf_P with %S


void ScreenSettings::ProcessUpdateAction(const Context* context)
{
    current++;
    if (current >= MENUITEMCOUNT)
      current = 0;
}

void ScreenSettings::ProcessCommitAction(const Context* context)
{
   switch(current)
   {
    case 0:  //time
      context->GotoTimeEditScreen();
      break;
    case 1: //date
      context->GotoDateEditScreen();
      break;
    case 2: //clk
      context->SetIs24Hour(!context->GetIs24Hour());
      break;
    case 3: //metric
      context->SetIsMetric(!context->GetIsMetric());
      break;
    case 4: //Auto channel change
      context->SetIsAutoChannelChange(!context->GetIsAutoChannelChange());
      break;
    default: //exit
      context->GotoTimeScreen();
      break;
   }
}

void ScreenSettings::OnShow(const Context* context)
{
    current = 0;
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

void ScreenSettings::Render(const Context* context)
{
  context->drawRoundRect(x0, y0, w, h, radius, color);
  
  char data2[ARRAY_SIZE];
  sprintf_P(data2, PSTR("%S"), menuItems[current]);

  if (current == 2)
  {
    char data3[10];
    for(int x=0;x<ARRAY_SIZE;x++)
      data3[x]=data2[x];
    sprintf(data2, data3, context->GetIs24Hour() ? "24" : "12");
  }
  else if (current == 3)
  {
    char data3[ARRAY_SIZE];
    for(int x=0;x<ARRAY_SIZE;x++)
      data3[x]=data2[x];
    sprintf(data2, data3, context->GetIsMetric() ? "C" : "F");
  }
  else if (current == 4)
  {
    char data3[ARRAY_SIZE];
    for(int x=0;x<ARRAY_SIZE;x++)
      data3[x]=data2[x];
    sprintf(data2, data3, context->GetIsAutoChannelChange() ? "on" : "off");
  }
  
  int x = 64;
  int y = 32;
  context->drawText(data2, 2, x, y, center, false);
}
