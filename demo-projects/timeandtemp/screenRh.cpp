#include "commonTypes.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include "context.h"
#include "screen.h"
#include "screenRh.h"

void ScreenRh::ProcessUpdateAction(const Context* context)
{
 context->GotoTimeScreen();
}

void ScreenRh::Render(const Adafruit_SSD1306* display, const Context* context)
{
  //display humidity
  char data[10];
  dtostrf(context->GetPercentRh(), 3, 1, data);
  sprintf_P(data, PSTR("%s %%"), data);

  int x = 64;
  int y = 40;
  drawText(display, data, 3, x, y, center, false);
}
