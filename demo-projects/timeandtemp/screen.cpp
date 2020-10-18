#include "commonTypes.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include "context.h"
#include "screen.h"

void Screen::ProcessUpdateAction(const Context* context)
{
	
};

void Screen::ProcessCommitAction(const Context* context)
{
   context->GotoSettingsScreen();
}

void Screen::OnShow(const Context* context)
{
  
}

void Screen::Render(const Adafruit_SSD1306* display, const Context* context)
{
	
}

bool Screen::AllowAutoChannelChange()
{
  return true;
}

void Screen::Tick()
{
  
}

//Use this function if the text pointer is pointing to program memory.
void Screen::drawText_P(const Adafruit_SSD1306* display, const char* text, int textSize, int16_t &x, int16_t &y, ALIGN align, bool superscript) 
{
     char* buf = new char[strlen_P(text)+1];
     strcpy_P(buf, text);
     drawText(display, buf, textSize, x, y, align, superscript);
     delete[] buf;
}

void Screen::drawText(const Adafruit_SSD1306* display, const char* text, int textSize, int16_t &x, int16_t &y, ALIGN align, bool superscript)
{
  int16_t x1 = 0, y1 = 0;
  uint16_t w = 0, h = 0;
  display->setCursor(0,0);
  display->setTextWrap(false);
  display->setTextColor(WHITE);
  display->setTextSize(textSize);
  display->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  
  switch(align)
  {
    case left:
    
    break;
    case center:
      x -= w / 2;
      y -= h / 2;
    break;
    
    case right:
      x -= w;
      y -= superscript ? 2 * h : h;
    break;
  }

  display->setCursor(x, y);
  display->print(text);
}
