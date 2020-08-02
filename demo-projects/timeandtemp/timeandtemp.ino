#include <Adafruit_SSD1306.h>
#include <RTClib.h>
//#include <SparkFunBME280.h>

//Data type definitions
typedef struct 
{
  float a;      //initialization of EMA alpha
  float s;      //initialization of EMA S
} EMA;

typedef enum
{
  left,
  center,
  right
} ALIGN;

typedef struct
{
  void (*buttonUp)(bool);
  void (*beginLongPress)();
  void (*render)();
}SCREEN;

SCREEN full = {&ShowDate, &EnterSetTime, &RenderFull};
SCREEN date = {&ShowTime, &EnderSetDate, &RenderDate};
SCREEN editTime = {&EditTimeField, &SaveTimeField, &RenderEditTime};
volatile SCREEN currentScreen;

//Constants
#define OLED_RESET 4
#define BUTTON_PIN 2
#define RTC_SQW_PIN 3
#define RTC_32K_PIN 5
#define TEMP_CORRECTION -10.0

const char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

//Variables
volatile DateTime dateTime;
volatile long lastFall = 0;

//I2C devices
Adafruit_SSD1306 display(OLED_RESET);
RTC_DS3231 rtc;
//BME280 bme280;

//BME280 readings
EMA percentRH {0.1, 0};
EMA temperature {0.9, 0};
EMA altitude {0.1, 0};
EMA pressure {0.1, 0};

EMA button {0.5, 0};

//Exponential Moving Average
//https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
void UpdateEma(EMA& ema, float sample)
{
  ema.s = (ema.a * sample) + ((1-ema.a)*ema.s);    //run the EMA
}

void setup () 
{
  //Configure the RTC
  if (! rtc.begin()) 
  {
    while (1);
  }

  //No way to set time other than to uncomment this and upload to device.  Then uncomment and re-upload.
  //if (rtc.lostPower()) {
   // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //}

  rtc.writeSqwPinMode(Ds3231SqwPinMode::DS3231_SquareWave1Hz);
  dateTime = rtc.now();

  //Initialize the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  //Configure the BME2800
/*  bme280.settings.commInterface = I2C_MODE;
  bme280.settings.I2CAddress = 0x76;
  bme280.settings.runMode = 3; //Normal mode
  bme280.settings.tStandby = 0;
  bme280.settings.filter = 0;
  bme280.settings.tempOverSample = 1;
  bme280.settings.pressOverSample = 1;
  bme280.settings.humidOverSample = 1;
  bme280.begin();
*/
  //Start the serial comm
  Serial.begin(9600);

  //Setup input pins
  pinMode(RTC_SQW_PIN, INPUT);         //Square wave from rtc
  pinMode(RTC_32K_PIN, INPUT);        //32K from rtc, useless as no interop on nano
  pinMode(BUTTON_PIN, INPUT_PULLUP);  //Button pin

  //Prime the first value to avoid initial homing of average from zero.
/*  percentRH.s = bme280.readFloatHumidity();
  temperature.s = bme280.readTempF() + TEMP_CORRECTION;
  altitude.s = bme280.readFloatAltitudeFeet();
  pressure.s = bme280.readFloatPressure();
*/
  //Setup interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), pin2ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(RTC_SQW_PIN), pin3ISR, RISING);

  currentScreen = full;
}

void pin3ISR()
{
  interrupts(); //Enable interrupts so that I2C communication can work, equivalent to sei();
dateTime = rtc.now();
/*  UpdateEma(temperature, bme280.readTempF() + TEMP_CORRECTION);
  UpdateEma(percentRH, bme280.readFloatHumidity());
  UpdateEma(pressure, bme280.readFloatPressure());
  UpdateEma(altitude, bme280.readFloatAltitudeFeet());*/
  currentScreen.render();
  
}

void pin2ISR()
{/*
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

  Serial.println("down");*/
}

int buttonV = 1;
int buttonUp = true;
float last = 0;
bool longPress = false;
void loop () 
{
  int value =  digitalRead(BUTTON_PIN);
  UpdateEma(button, value);

  if ((int)button.s != 0 && (int)button.s != 1)
  {
    //button in transition
    return;
  }

  if (buttonV != (int)button.s)
  {
    if (button.s == 1)
    {
      ButtonUp(longPress);
      longPress = false;
      buttonUp = true;
    }
    else
    {
      buttonUp = false;
      last = millis();
    }

    buttonV = (int)button.s;
  }

  if (buttonUp == false)
  {
      float now = millis();
      if (now - last > 2000 && !longPress)
      {
        longPress = true; 
        LongPressBegin();
      }
  }



 // delay(100);
}

float angle = 0;

void ButtonUp(bool longPress)
{
  Serial.print("click up - ");
  Serial.println(longPress ? "long" : "short");  
  currentScreen.buttonUp(longPress);
}

void LongPressBegin()
{
   Serial.println("long press start");
   currentScreen.beginLongPress();
}

//button up
void EditTimeField(bool isLongPress)
{
}

void SaveTimeField()
{
  currentScreen=full;
}

void RenderEditTime()
{
  Serial.println("render edit time");
  display.clearDisplay();
  int x = 64;
  int y = 32;
  drawText("edit time", 2, x, y, center, false);
  display.display();
}

void ShowDate(bool isLongPress)
{
  if (!isLongPress)
  {
    currentScreen = date;
  }
}

void EnterSetTime()
{
  currentScreen = editTime;
}

void ShowTime(bool isLongPress)
{
  if (!isLongPress)
  {
    currentScreen = full;
  }
}

void EnderSetDate()
{
  //currentScreen = 
}

void RenderDate()
{
  Serial.println("render date");
  display.clearDisplay();
  //display.setTextColor(WHITE);
  //display.setCursor(0,0);

   //display date
  char data2[17];
  sprintf(data2, "%s\n%02d/%02d\n%d", daysOfTheWeek[dateTime.dayOfTheWeek()], dateTime.month() ,dateTime.day(), dateTime.year());
 Serial.println(data2);
  int x = 64;
  int y = 32;
  drawText(data2, 2, x, y, center, false);
  Serial.print((int)x);
  Serial.print((int)y);

  display.display();
}

void RenderFull()
{
  Serial.println("render full");
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextWrap(false);

  // Display time
  char data[6];
  sprintf(data, "%02d:%02d", dateTime.hour(), dateTime.minute());
  String a(data);
  int16_t x = 65;
  int16_t y = 12;
  drawText(a, 3, x, y, center, false);

  //Display temp
  char data3[10];
  dtostrf(temperature.s, 3, 1, data3);
  sprintf(data3, "%sF", data3);
  x = 0;
  y = 63 - 16;
  drawText(data3, 2, x, y, left, false);

  //display % humidity
  x = SSD1306_LCDWIDTH - 1;
  y = SSD1306_LCDHEIGHT -1;

  drawText("%", 1, x, y, right, true);
  y = SSD1306_LCDHEIGHT -1;
  drawText(String(percentRH.s, 1), 2, x, y, right, false);

  //display date
  char data2[17];
  sprintf(data2, "%s, %d/%02d/%02d", daysOfTheWeek[dateTime.dayOfTheWeek()], dateTime.year(), dateTime.month() ,dateTime.day());
 
  x = 64;
  y = 32;
  drawText(data2, 1, x, y, center, false);

  //Things to experiement with...
  //setTextWrap
  //fillScreen
  //drawLine
  //drawRect
  //drawPixel
  //Adafruit_GFX_Button

  //1 is default 6x8, 2 is 12x16, 3 is 18x24, etc

 // display.drawRoundRect(0, 0, SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT, /* radius*/ 4, /*color*/ 1);

  const float radSecond = 0.10472;

  angle = (radSecond * dateTime.second()) - (15 * radSecond);

  //Serial.println(angle * 180/PI);
   
  float x2 = cos(angle);
  float y2 = sin(angle);

  display.drawLine(SSD1306_LCDWIDTH / 2, SSD1306_LCDHEIGHT/2, (x2*64) + SSD1306_LCDWIDTH / 2, (y2*64) + SSD1306_LCDHEIGHT/2, 1);
  
  display.display();

  //Serial.println(angle);
  //Serial.println();
}

void drawText(const String text, int textSize, int16_t &x, int16_t &y, ALIGN align, bool superscript)
{
  int16_t x1 = 0, y1 = 0;
  uint16_t w = 0, h = 0;
  display.setTextSize(textSize);
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  
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

  display.setCursor(x, y);
  display.print(text);
}
