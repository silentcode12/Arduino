#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <SparkFunBME280.h>

volatile int textSize = 3;

#define OLED_RESET 4
#define BUTTON_PIN 2
#define RTC_SQW_PIN 3
#define RTC_32K_PIN 5

Adafruit_SSD1306 display(OLED_RESET);
RTC_DS3231 rtc;
BME280 bme280;

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

typedef struct 
{
  float a;      //initialization of EMA alpha
  float s;      //initialization of EMA S
} EMA;

EMA percentRH {0.1, 0};
EMA tempF {0.9, 0};
EMA altMeters {0.1, 0};
EMA altFeet {0.1, 0};
EMA preasureKpa {0.1, 0};

//Exponential Moving Average
//https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
void UpdateEma(EMA& ema, float sample)
{
  ema.s = (ema.a * sample) + ((1-ema.a)*ema.s);    //run the EMA
}

void setup () {
  if (! rtc.begin()) {
    while (1);
  }

  //No way to set time other than to uncomment this and upload to device.  Then uncomment and re-upload.
  //if (rtc.lostPower()) {
   // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //}

  rtc.writeSqwPinMode(Ds3231SqwPinMode::DS3231_SquareWave1Hz);
  
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

  pinMode(RTC_SQW_PIN, INPUT);         //Square wave from rtc
  pinMode(RTC_32K_PIN, INPUT);        //32K from rtc, useless as no interop on nano
  pinMode(BUTTON_PIN, INPUT_PULLUP);  //Button pin

  //Prime the first value to avoid initial homing of average from zero.
  percentRH.s = bme280.readFloatHumidity();
  tempF.s = bme280.readTempF();
  altFeet.s = bme280.readFloatAltitudeFeet();
  preasureKpa.s = bme280.readFloatPressure();

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), pin2ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(RTC_SQW_PIN), pin3ISR, FALLING);
}

volatile long lastFall = 0;

void pin3ISR(){
  Serial.println("pin2ISR");  
}

void pin2ISR(){
  volatile int currentValue = digitalRead(BUTTON_PIN);

  if (currentValue == HIGH)
    return;  //We were falling but now we're high

  volatile long now = millis();

  if (now - lastFall < 500)
  {
      return;  //too quick, likely bounce, ignore
  }

lastFall = now;
currentValue = digitalRead(BUTTON_PIN);
if (currentValue == HIGH)
    return;  //We were falling but now we're high

  Serial.println("down");

  if (textSize + 1 > 4)
    textSize = 1;
  else
    textSize++;
}

void loop () {
  DateTime now = rtc.now();

	display.clearDisplay();
	display.setTextColor(WHITE);
	display.setCursor(0,0);

  if (textSize == 4)
  {
    display.setTextSize(1);
    display.println();
    display.println();
  }
  else if (textSize == 3)
  {
    display.setTextSize(1);
    display.println();
  }
   else if (textSize == 2)
  {
    display.setTextSize(1);
    display.println();
  }
   else if (textSize == 1)
  {
    display.setTextSize(1);
    display.println();
    display.println();
    display.println();
  }
  
  display.setTextSize(textSize);
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

  int supTextSize = 2;

  if (textSize < 3)
    supTextSize = 1;

  display.setTextSize(1);
  display.println();
  display.setTextSize(supTextSize);

  UpdateEma(tempF, bme280.readTempF() - 10.0);
  display.print(tempF.s, 1);
  display.setTextSize(1);
  display.print("F ");

  UpdateEma(percentRH, bme280.readFloatHumidity());
  display.setTextSize(supTextSize);
  display.print(percentRH.s, 1);
  display.setTextSize(1);
  display.println("%");

  if (textSize < 3)
  {
    display.setTextSize(supTextSize);
    UpdateEma(preasureKpa, bme280.readFloatPressure());
    display.print(preasureKpa.s/1000.0, 2);
    display.print(" KPa, ");
  
    UpdateEma(altFeet, bme280.readFloatAltitudeFeet());
    display.print(altFeet.s, 2);
    display.println(" ft"); 
  }
  
  display.display();
  delay(250);
}

void printNumber(int number)
{
  if (number < 10) 
    display.print("0");
    
  display.print(number, DEC);
}
