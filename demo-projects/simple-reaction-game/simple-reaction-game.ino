//Adapted from the SunFounder projects LED Flowing Lights, Controlling LED with button, Controlling LED with potentiometer
const int lowestPin = 4;
const int highestPin = 11;
const int defaultDelay = 500;
volatile int delayTime = defaultDelay;
const int stepInterval = 20;
volatile int currentPin = 0;

int sensorPin = A0;

void setup()
{
  for(int thisPin = lowestPin; thisPin <= highestPin; thisPin++)
  {
     pinMode(thisPin, OUTPUT);
  }
  
  attachInterrupt(0, resetButtonPush, FALLING);
  attachInterrupt(1, fireButtonPush, FALLING);
  
  Serial.begin(9600);  
  Serial.print("Hello:\r\n"); 
} 

int getDelay(){
  int inputValue = analogRead(sensorPin);
  int v = map(inputValue, 0, 1023, 25, 255);
  Serial.print("input value: ");
  Serial.println(v);
  return v;
}

void loop()
{  
  for(int thisPin = lowestPin;thisPin <= highestPin;thisPin++)
  {
    noInterrupts();
    digitalWrite(thisPin, HIGH);
    if (thisPin - lowestPin > 0) digitalWrite(thisPin -1, LOW);
    currentPin = thisPin;
    interrupts();
    delay(delayTime);
  }
  
  for(int thisPin = highestPin; thisPin>=lowestPin; thisPin--)
  {
    noInterrupts();
    digitalWrite(thisPin, HIGH);
    if (highestPin - thisPin > 0) digitalWrite(thisPin + 1, LOW);
    currentPin = thisPin;
    interrupts();
    delay(delayTime);
  }
 }

void resetButtonPush() {
  delayTime = defaultDelay;
  delayTime = getDelay();
}

void levelUp(){
  if (delayTime - stepInterval > 0){
    delayTime -= stepInterval;
  } 
  else{
    delayTime = 100;
  }
}

void playMiss(){
  tone(13, 200, 250);
}

void playHit(){
  tone(13, 8000,250);
}

void fireButtonPush() {
  noTone(13);
  if (currentPin == 7){
    playHit();
    levelUp();
  }
  else{
    playMiss();
  }
}
