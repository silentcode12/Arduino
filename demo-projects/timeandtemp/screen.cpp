#include "arduino.h"
#include "context.h"
#include "screen.h"

Screen::~Screen()
{
}

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

void Screen::Render(const Context* context)
{
	
}

bool Screen::AllowAutoChannelChange()
{
  return true;
}

void Screen::Tick()
{
  
}
