// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <SparkFunBME280.h>

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

int key = ZERO;
volatile int textSize = 2;

#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);
RTC_DS3231 rtc;
BME280 bme280;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
  if (! rtc.begin()) {
    while (1);
  }

  //if (rtc.lostPower()) {
   // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //}
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  bme280.settings.commInterface = I2C_MODE;
  bme280.settings.I2CAddress = 0x76;
  bme280.settings.runMode = 3; //Normal mode
  bme280.settings.tStandby = 0;
  bme280.settings.filter = 0;
  bme280.settings.tempOverSample = 1;
  bme280.settings.pressOverSample = 1;
  bme280.settings.humidOverSample = 1;
  bme280.begin();

  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(2), pin2ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), pin3ISR, FALLING);
}

volatile long lastFall = 0;

void pin2ISR(){
  volatile int currentValue = digitalRead(2);

  if (currentValue == HIGH)
    return;  //We were falling but now we're high

  volatile long now = millis();

  if (now - lastFall < 500)
  {
      return;  //too quick, likely bounce, ignore
  }

lastFall = now;
currentValue = digitalRead(2);
if (currentValue == HIGH)
    return;  //We were falling but now we're high

  Serial.println("down");

  if (textSize + 1 > 4)
    textSize = 1;
  else
    textSize++;
}

void pin3ISR(){
  Serial.println("pin3ISR");
}

void processKeyPress(int key)
{
  switch(key)
    {
      case ONE: 
      {
        textSize=1;
        Serial.println("ONE");
        break;
      }
      case TWO:
      {
        textSize=2;
        Serial.println("TWO");
        break;
      }
      case THREE:
      {
        textSize=3;
        Serial.println("THREE");
        break;
      }
      case FOUR: 
      {
        textSize=4;
        Serial.println("FOUR");
        break;
      }
      case FIVE:
      {
        Serial.println("FIVE");
        break;
      }
      case SIX:
      {
        Serial.println("SIX");
        break;
      }
      case SEVEN:
      {
        Serial.println("SEVEN");
        break;
      }
      case EIGHT:
      {
        Serial.println("EIGHT");
        break;
      }
      case NINE:
      {
        Serial.println("NINE");
        break;
      }
      case ZERO:
      {
        Serial.println("ZERO");
        break;
      }
      case UP:
      {
        Serial.println("UP");
        break;
      }
      case DOWN:
      {
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

void loop () {
  DateTime now = rtc.now();

	display.clearDisplay();
  
  display.setTextSize(textSize);
	display.setTextColor(WHITE);
	display.setCursor(0,0);

  display.print("");
  printNumber(now.hour());
  display.print(':');
  printNumber(now.minute());
  if (textSize == 1)
    display.print(':');
  
  if (textSize < 4){
  display.setTextSize(1);
  printNumber(now.second());
  }
  
  for(int x = textSize; x > 0; x--)
  {
    display.println();
  }

  display.print(daysOfTheWeek[now.dayOfTheWeek()]);

  display.print(" ");
  display.print(now.year(), DEC);
  display.print('/');
  printNumber(now.month());
  display.print('/');
  printNumber(now.day());
  display.println();

  display.setTextSize(1);
  //display.print(bme280.readTempC(), 2);
  //display.print("C, ");

  display.print(bme280.readTempF(), 1);
  display.setTextSize(1);
  display.print("  o");
  display.setTextSize(1);
  display.print("F ");

  display.print(bme280.readFloatHumidity(), 1);
  display.println(" %RH");
  
  display.print(bme280.readFloatPressure()/1000, 2);
  display.print(" KPa, ");

  display.print(bme280.readFloatAltitudeMeters(), 2);
  display.print("m, ");

  display.print(bme280.readFloatAltitudeFeet(), 2);
  display.println("ft"); 

  display.display();
  delay(125);

  if (irrecv.decode(&results)) {

    if (results.value != REPEAT_KEY)
    {
      key = results.value;
    }
    processKeyPress(key);
    irrecv.resume();
  }
}

void printNumber(int number)
{
  if (number < 10) display.print("0");
  display.print(number, DEC);
}
