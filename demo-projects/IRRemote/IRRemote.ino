/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

#define LEFT 0xFF22DD
#define RIGHT 0xFFC23D
#define UP 0xFF629D
#define DOWN 0xFFA857
#define OK 0xFF02FD

#define ONE 0xFF6897
#define TWO 0xFF9867
#define THREE 0xFFB04F
#define FOUR 0xFF30CF
#define FIVE 0xFF18E7
#define SIX 0xFF7A85
#define SEVEN 0xFF10EF
#define EIGHT 0xFF38C7
#define NINE 0xFF5AA5
#define ZERO 0xFF4AB5

#define STAR 0xFF42BD
#define HASH 0xFF52AD

#define REPEAT_KEY 0xFFFFFFFF


int RECV_PIN = 7;

IRrecv irrecv(RECV_PIN);

decode_results results;

int brightness = 125;
int led = 10; 
int step = 5;
int key = ZERO;

void setup()
{
  
  pinMode(11, OUTPUT);
  analogWrite(led, brightness);
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void dump(decode_results *results) {
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  }
  else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  }
  else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  }
  else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  }
  else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->address,HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == LG) {
     Serial.print("Decoded LG: ");
  }
  else if (results->decode_type == JVC) {
     Serial.print("Decoded JVC: ");

  }
  else if (results->decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  }
  else if (results->decode_type == WHYNTER) {
     Serial.print("Decoded Whynter: ");
  }
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (int i = 0; i < count; i++) {
    if ((i % 2) == 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    }
    else {
      Serial.print(-(int)results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println("");
}

void updateLED(int key)
{
  switch(key)
    {
      case ONE: 
      {
        brightness = 25;
        analogWrite(led, brightness);
        Serial.println("ONE");
        break;
      }
      case TWO:
      {
        brightness = 50;
        analogWrite(led, brightness);
        Serial.println("TWO");
        break;
      }
      case THREE:
      {
        brightness = 75;
        analogWrite(led, brightness);
        Serial.println("THREE");
        break;
      }
      case FOUR: 
      {
        brightness = 100;
        analogWrite(led, brightness);
        Serial.println("FOUR");
        break;
      }
      case FIVE:
      {
        brightness = 125;
        analogWrite(led, brightness);
        Serial.println("FIVE");
        break;
      }
      case SIX:
      {
        brightness = 150;
        analogWrite(led, brightness);
        Serial.println("SIX");
        break;
      }
      case SEVEN:
      {
        brightness = 175;
        analogWrite(led, brightness);
        Serial.println("SEVEN");
        break;
      }
      case EIGHT:
      {
        brightness = 200;
        analogWrite(led, brightness);
        Serial.println("EIGHT");
        break;
      }
      case NINE:
      {
        brightness = 255;
        analogWrite(led, brightness);
        Serial.println("NINE");
        break;
      }
      case ZERO:
      {
        brightness = 0;
        analogWrite(led, brightness);
        Serial.println("ZERO");
        break;
      }
      case UP:
      {
        if (brightness + step >= 255)
          brightness = 255;
        else
          brightness += step;
        analogWrite(led, brightness);
        Serial.println("UP");
        break;
      }
      case DOWN:
      {
        if (brightness - step <= 0)
          brightness = 0;
        else
          brightness -= step;
        analogWrite(led, brightness);
        Serial.println("DOWN");
        break;
      }
      case OK:
      {
        Serial.println("OK");
        break;
      }
      case STAR:
      {
        Serial.println("STAR");
        break;
      }
      case HASH:
      {
        Serial.println("HASH");
        break;
      }
      case LEFT:
      {
        Serial.println("LEFT");
        break;
      }
      case RIGHT:
      {
        Serial.println("RIGHT");
        break;
      }
      case REPEAT_KEY:
      {
        Serial.println("REPEAT_KEY");
        break;
      }
    }      
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    dump(&results);

    if (results.value != REPEAT_KEY)
    {
      key = results.value;
    }
    updateLED(key);
    irrecv.resume();
    
  }

  //delay(100);
}
