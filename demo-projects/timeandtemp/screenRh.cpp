#include "commonTypes.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <SparkFunBME280.h>
#include <RTClib.h>
#include "context.h"
#include "screen.h"
#include "screenRh.h"

void ScreenRh::ProcessCommitAction(const Context* context)
{
   context->GotoSettingsScreen();
}

void ScreenRh::ProcessUpdateAction(const Context* context)
{
 context->GotoTimeScreen();
}

void ScreenRh::OnShow(const Context* context)
{}

void ScreenRh::Render(const Adafruit_SSD1306* display, const Context* context)
{
  int x, y;
  x = y = 10;
  //drawText_P(display, PSTR("%RH"), 1, x, y, left, false);

  //display humidity
  char data[10];
  dtostrf(context->GetPercentRh().s, 3, 1, data);
  sprintf_P(data, PSTR("%s %%"), data);

  x = 64;
  y = 40;
  drawText(display, data, 3, x, y, center, false);
}
