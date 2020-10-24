#include "arduino.h"
#include "context.h"
#include "screen.h"
#include "screenRh.h"

void ScreenRh::ProcessUpdateAction(const Context* context)
{
 context->GotoTimeScreen();
}

void ScreenRh::Render(const Context* context)
{
  //display humidity
  char data[10];
  dtostrf(context->GetPercentRh(), 3, 1, data);
  sprintf_P(data, PSTR("%s %%"), data);

  context->drawText(data, 3, 64, 40, center, false);
}
