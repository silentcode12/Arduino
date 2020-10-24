#include "arduino.h"
#include "context.h"
#include "screen.h"
#include "screenTemp.h"

void ScreenTemp::ProcessUpdateAction(const Context* context)
{
   context->GotoRhScreen();
}

void ScreenTemp::Render(const Context* context)
{
  //display temp
  char data[10];

  float temp = context->GetTemperatureF();

  if (context->GetIsMetric())
  {
    temp = (temp - 32) * 5.0/9.0;
    dtostrf(temp, 3, 1, data);
    sprintf_P(data, PSTR("%s C"), data);
  }
  else
  {
    dtostrf(temp, 3, 1, data);
    sprintf_P(data, PSTR("%s F"), data);
  }
  
  context->drawText(data, 3, 64, 40, center, false);
}
