#include "commonTypes.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include "context.h"
#include "screen.h"
#include "screenTemp.h"

void ScreenTemp::ProcessUpdateAction(const Context* context)
{
   context->GotoRhScreen();
}

void ScreenTemp::Render(const Adafruit_SSD1306* display, const Context* context)
{
  //display temp
  char data[10];

  float temp = context->GetTemperature();

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
  
  int x = 64;
  int y = 40;
  drawText(display, data, 3, x, y, center, false);
}
