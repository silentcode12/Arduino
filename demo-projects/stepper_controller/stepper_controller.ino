//Simple project to allow GRBL control of a 5V Stepper Motor 28BYJ-48 Drive Test Module Board ULN2003 5 Line 4 Phase
#include <Stepper.h>

//Connect to pin     IN 1   2  3   4 on driver board
Stepper myStepper(2048, 8, 10, 9, 11);

// Connect to x, y or z axis direction pin on GRBL board
int axisDirectionPin= 7;

void setup()
{  
  pinMode(axisDirectionPin, INPUT); 
  
  //Detect falling edge of GRBL axis step pulse (x, y or z)
  //Connect pin to x, y or z axis step pulse pin on GRBL board
  attachInterrupt(0, interrupt, FALLING);
}

void loop()
{
}

void interrupt()
{
  //Step one position in the specified direction
  if(digitalRead(axisDirectionPin) == HIGH) 
  {
	myStepper.step(1);
  }
  else
  {
	myStepper.step(-1);
  }
}

