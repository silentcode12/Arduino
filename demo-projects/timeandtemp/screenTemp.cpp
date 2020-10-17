#include "commonTypes.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <SparkFunBME280.h>
#include <RTClib.h>
#include "context.h"
#include "screen.h"
#include "screenTemp.h"

void ScreenTemp::ProcessUpdateAction(const Context* context)
{
   context->GotoRhScreen();
}

void ScreenTemp::Render(const Adafruit_SSD1306* display, const Context* context)
{
  int x, y;
  x = y = 10;
  
  //display temp
  char data[10];
  dtostrf(context->GetTemperature(), 3, 1, data);
  sprintf_P(data, PSTR("%s F"), data);

  x = 64;
  y = 40;
  drawText(display, data, 3, x, y, center, false);
}
