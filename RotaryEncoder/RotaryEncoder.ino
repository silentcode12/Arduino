void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), isr2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), isr3, LOW);
}

volatile unsigned long last = 0;
void isr2(){
  //Serial.println("2");

  int b = millis();
  if (b - last < 5) return;
  last = millis();

  int a = digitalRead(3);
  if (a == LOW)
    Serial.println("-");
  else 
    Serial.println("+");
}

void isr3(){
  //Serial.println("3");
}

void loop() {
  // put your main code here, to run repeatedly:
  int sw = digitalRead(4);
  if (sw == LOW)
    Serial.println("sw low");
}
