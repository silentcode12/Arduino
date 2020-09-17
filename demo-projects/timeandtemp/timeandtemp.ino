#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <SparkFunBME280.h>
#include <EEPROM.h>

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

struct SETTINGS
{
  bool is24hr;
  bool isMetric;
};

typedef struct
{
  void (*buttonUp)(bool);
  void (*beginLongPress)();
  void (*render)();
}SCREEN;

//Display screen definitions
SCREEN timeScreen = {&ShowDate, &EnterSetTime, &RenderTime};
SCREEN dateScreen = {&ShowTemp, &EnterSetDate, &RenderDate};
SCREEN tempScreen = {&ShowPercentRh, &EnterSetTemp, &RenderTemp};
SCREEN percentRhScreen = {&ShowTime, &EnterSetPercentRh, &RenderPercentRh};

//Edit screen defintions
SCREEN editTimeScreen = {&EditTimeField, &SaveTimeField, &RenderEditTime};
SCREEN editDateScreen = {&EditDateField, &SaveDateField, &RenderEditDate};

volatile SCREEN currentScreen;

//Constants
#define OLED_RESET 4
#define BUTTON_PIN 2
#define RTC_SQW_PIN 3
#define RTC_32K_PIN 5
#define TEMP_CORRECTION -10.0

const char daysOfTheWeek[7][4] PROGMEM = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  //Stored in flash, read out using sprintf_P with %S

//Variables
int timeIndex = 0;
int dateIndex = 0;
byte buttonState = 1;
bool buttonUp = true;
bool longPress = false;
short time[] = {0, 0, 0};
float last = 0;
volatile DateTime dateTime;
//volatile long lastFall = 0;

//I2C devices
Adafruit_SSD1306 display(OLED_RESET);
RTC_DS3231 rtc;
BME280 bme280;

//BME280 readings
EMA percentRH {0.1, 0};
EMA temperature {0.9, 0};
EMA altitude {0.1, 0};
EMA pressure {0.1, 0};

EMA button {0.5, 1};

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

  rtc.writeSqwPinMode(Ds3231SqwPinMode::DS3231_SquareWave1Hz);
  dateTime = rtc.now();

  //Initialize the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  //Configure the BME2800
  bme280.settings.commInterface = I2C_MODE;
  bme280.settings.I2CAddress = 0x76;
  bme280.settings.runMode = 3; //Normal mode
  bme280.settings.tStandby = 0;
  bme280.settings.filter = 0;
  bme280.settings.tempOverSample = 1;
  bme280.settings.pressOverSample = 1;
  bme280.settings.humidOverSample = 1;
  bme280.begin();

  //Setup input pins
  pinMode(RTC_SQW_PIN, INPUT);         //Square wave from rtc
  pinMode(RTC_32K_PIN, INPUT);        //32K from rtc, useless as no interop on nano
  pinMode(BUTTON_PIN, INPUT_PULLUP);  //Button pin

  //Prime the first value to avoid initial homing of average from zero.
  percentRH.s = bme280.readFloatHumidity();
  temperature.s = bme280.readTempF() + TEMP_CORRECTION;
  altitude.s = bme280.readFloatAltitudeFeet();
  pressure.s = bme280.readFloatPressure();

  //Setup interrupts
 // attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), pin2ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(RTC_SQW_PIN), pin3ISR, RISING);

  currentScreen = timeScreen;
}

void pin3ISR()
{
  static boolean IsRunning = false;
  if(IsRunning) 
    return;
  
  IsRunning = true;
  
  interrupts(); //Enable interrupts so that I2C communication can work, equivalent to sei();
  
  dateTime = rtc.now();
  UpdateEma(temperature, bme280.readTempF() + TEMP_CORRECTION);
  UpdateEma(percentRH, bme280.readFloatHumidity());
  UpdateEma(pressure, bme280.readFloatPressure());
  UpdateEma(altitude, bme280.readFloatAltitudeFeet());

  currentScreen.render();

  noInterrupts();
  IsRunning = false;
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

void UpdateImmediate()
{
  noInterrupts();
  pin3ISR();
  interrupts();
}

void playAnimation()
{
  noInterrupts();
  detachInterrupt(digitalPinToInterrupt(RTC_SQW_PIN));
  interrupts();
  
  for (int vHoldPos = SSD1306_LCDHEIGHT; vHoldPos > 0; vHoldPos--)
  {  
    for (int x = 0; x < SSD1306_LCDWIDTH; x += 4)
    {
      for (int y = 0; y < SSD1306_LCDHEIGHT; y += 4)
      {
        long color = random(0, 4) % 2;
        display.fillRect(x, y, 4, 4, color);
      }
    }

    display.fillRect(0, vHoldPos, SSD1306_LCDWIDTH, 15, 0);

    vHoldPos -= 4;
    
    display.display();
  }

  attachInterrupt(digitalPinToInterrupt(RTC_SQW_PIN), pin3ISR, RISING);
}

void loop () 
{
  UpdateEma(button, digitalRead(BUTTON_PIN));

  if ((byte)button.s != 0 && (byte)button.s != 1)
  {
    //button in transition
    return;
  }

  if (buttonState != (byte)button.s)
  {
    if (button.s == 1)
    {
      ButtonUp(longPress);
      UpdateImmediate();
      longPress = false;
      buttonUp = true;
    }
    else
    {
      buttonUp = false;
      last = millis();
    }

    buttonState = (bool)button.s;
  }

  if (buttonUp == false)
  {
      float now = millis();
      if (now - last > 2000 && !longPress)
      {
        longPress = true; 
        LongPressBegin();
        UpdateImmediate();
      }
  }
}

void ButtonUp(bool isLongPress)
{
  currentScreen.buttonUp(isLongPress);
}

void LongPressBegin()
{
   currentScreen.beginLongPress();
}

bool IsLeapYear(short year)
{
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ?
      true :
      false;
}

//EEPROM memory mappings
SETTINGS settings EEMEM = {false, false}; //todo:  Figure out how to initialize eeprom memory at flash time.  looks like a separate binary file...

SETTINGS GetSettings()
{
  SETTINGS s;
  EEPROM.get((int)&settings, s);
  if (s.is24hr != 0 && s.is24hr != 1)
  {
    //EEPROM isn't initialized, do it now.
    s.is24hr = false;
    EEPROM.put((int)&settings, s);
  }
  
  return s;
}

void PutSettings(SETTINGS s)
{
  EEPROM.put((int)&settings, s);
}

//button up
void EditDateField(bool isLongPress)
{
    if (!isLongPress)
    {
      //Deconstruct the year, Todo: Only do this when updated year parts
      short thousands = (time[0] % 10000) / 1000;
      short hundreds = (time[0] % 1000) / 100;
      short tens = (time[0] % 100) / 10;
      short ones = time[0] %10;
    
      switch (dateIndex)
      {
        case 0:
          thousands++;
          if (thousands > 9)
            thousands = 1;
            
          break;  //update 1000s
        case 1:
          hundreds++;
          if (hundreds > 9)
            hundreds = 0;

          break;//update 100s
        case 2:
           tens++;
          if (tens > 9)
            tens = 0;

          break;//update 10s
        case 3:
           ones++;
          if (ones > 9)
            ones = 0;

          break;//update 1s
        case 4:
          time[1] += 1;
          if (time[1] > 12)
            time[1] = 1;
          break;//update month
        case 5:
          byte max = 0;
          switch(time[1])
          {
            case 2:
              max = IsLeapYear(time[0]) ? 29 : 28;
              break;
            case 4:
            case 6:
            case 9:
            case 11:
              max = 30;   //Apr4 Jun6 Sep9 Nov11
              break;
            default:
              max = 31; //Jan1 Mar3 May5 Jul7 Aug8 Oct10 Dec12
              break;
          }
          
          time[2] += 1;
          if (time[2] > max)
            time[2] = 1;
          break;//update day
      }

      //Reconstruct the year, Todo: Only do this when updated year parts
      time[0] = thousands * 1000 + hundreds * 100 + tens * 10 + ones;
   }
}

void SaveDateField()
{
  dateIndex++;
  switch (dateIndex)
  {
    case 0: break;
    case 1: break;
    case 2: break;
    case 3: break;
    case 4: break;
    case 5: break;
    default: 
    {
      DateTime newDateTime(time[0], time[1], time[2], dateTime.hour(), dateTime.minute(), dateTime.second());
      rtc.adjust(newDateTime);
      currentScreen = dateScreen;
      break;
    }
  }
}

void RenderEditDate()
{
  display.clearDisplay();
  int x, y;
  x = y = 10;
  drawText_P(PSTR("Edit date"), 1, x, y, left, false);
  
  int year = time[0];
  byte month = time[1];
  byte day = time[2];

  y += 10;
  char data[6];
  if(dateIndex <= 3)
  {
    //set year
    drawText_P(PSTR("Edit year"), 1, x, y, left, false);
    y += 10;

    sprintf_P(data, PSTR("%d"), year);

    drawText(data, 1, x, y, left, false);
    int w = 4;
    for(y=45; y <48; y ++)
    {
      x = 10;
      x = x 
      + (dateIndex * w) //numeric digits width
      + (dateIndex * w / 2);  //divider width
      int x1 = x + w;  // underline the two digits
      display.drawLine(x, y, x1, y, 1);
    }
  }
  else if (dateIndex == 4)
  {
    //set month
    drawText_P(PSTR("Edit month"), 1, x, y, left, false);
    y += 10;

     sprintf_P(data, PSTR("%d"), month);

    drawText(data, 1, x, y, left, false);
  }
  else if (dateIndex == 5)
  {
    //set day
    drawText_P(PSTR("Edit day"), 1, x, y, left, false);
    y += 10;

    sprintf_P(data, PSTR("%d"), day);

    drawText(data, 1, x, y, left, false);
  }

  display.display();
}

void EditTimeField(bool isLongPress)
{
  if (!isLongPress)
  {
    switch(timeIndex)
    {
      case -1:
        SETTINGS s;
        s = GetSettings();
        s.is24hr = !s.is24hr;
        //settings = s;
        PutSettings(s);
        return;
      case 0: 
        if (time[0] >= 23)
          time[0] = 0;
        break;
      case 1: 
        if(time[1] >= 59)
          time[1] = -1;
        break;
      case 2:
        time[2] = 0; //Note:  reset seconds to zero for synchronization, will count up
        return;
    }
    
    time[timeIndex]++;
  }
}

void SaveTimeField()
{
  timeIndex++;
  switch (timeIndex)
  {
    case -1: break;
    case 0: break;
    case 1: break;
    case 2: break;
    default: 
    {
      DateTime newDateTime(dateTime.year(), dateTime.month(), dateTime.day(), time[0], time[1], time[2]);
      rtc.adjust(newDateTime);
      currentScreen = timeScreen;
      break;
    }
  }
}

void RenderEditTime()
{
  display.clearDisplay();
  int x, y;
  x = y = 10;
  drawText_P(PSTR("Edit time"), 1, x, y, left, false);
  y += 20;

  if (timeIndex == -1)
  {
    //Display the clock mode (12hr/24hr) setting first
    x = 64;
    y = 32;
    drawText_P(GetSettings().is24hr ? PSTR("24hr") : PSTR("12hr"), 2, x, y, center, false);
  }
  else
  {
    char data[10];
    if (GetSettings().is24hr)
      sprintf_P(data, PSTR("%02d:%02d:%02d"), time[0], time[1], time[2]);
    else
      sprintf_P(data, PSTR("%02d:%02d:%02d %S"), time[0] > 12 ? time[0] - 12 : time[0], time[1], time[2], time[0] > 11 ? PSTR("P") : PSTR("A"));
      
    drawText(data, 2, x, y, left, false);

    //Draw a selection line under the active field.  Use to two digit string for width.
    int w, h;
    //display.setTextSize(1);
   // display.getTextBounds(PSTR("00"), 0, 0, &x, &y, &w, &h);
    w = 24;
    y = 45;
  
    for(y=45; y <48; y ++)
    {
      x = 10 
      + (timeIndex * w) //numeric digits width
      + (timeIndex * w / 2);  //divider width
      int x1 = x + w;  // underline the two digits
      display.drawLine(x, y, x1, y, 1);
    }
  }
  
  display.display();
}

void ShowDate(bool isLongPress)
{
  if (!isLongPress)
  {
    playAnimation();
    currentScreen = dateScreen;
  }
}

void ShowTemp(bool isLongPress)
{
  if (!isLongPress)
  {
    playAnimation();
    currentScreen = tempScreen;
  }
}

void EnterSetTemp()
{
  //do nothing for now
}

void EnterSetPercentRh()
{
  //do nothing for now
}

void EnterSetTime()
{
  timeIndex = -1;
  time[0] = dateTime.hour();
  time[1] = dateTime.minute();
  time[2] = dateTime.second();
  currentScreen = editTimeScreen;
}

void ShowTime(bool isLongPress)
{
  if (!isLongPress)
  {
    playAnimation();
    currentScreen = timeScreen;
  }
}

void ShowPercentRh(bool isLongPress)
{
  if (!isLongPress)
  {
    playAnimation();
    currentScreen = percentRhScreen;
  }
}

void EnterSetDate()
{
  dateIndex = 0;

  time[0] = dateTime.year();
  time[1] = dateTime.month();
  time[2] = dateTime.day();
  
  currentScreen = editDateScreen; 
}

void RenderDate()
{
  display.clearDisplay();
  int x, y;
  x = y = 10;
  drawText_P(PSTR("Date"), 1, x, y, left, false);
  
  //display date
  char data2[17];
  sprintf_P(data2, PSTR("%S %02d/%02d\n%d"), daysOfTheWeek[dateTime.dayOfTheWeek()], dateTime.month() ,dateTime.day(), dateTime.year());
  x = 0;
  y = 32;
  drawText(data2, 2, x, y, left, false);
  
  display.display();
}

void RenderTemp(){
  display.clearDisplay();
  int x, y;
  x = y = 10;
  drawText_P(PSTR("Temp"), 1, x, y, left, false);

  //display temp
  char data[10];
  dtostrf(temperature.s, 3, 1, data);
  sprintf_P(data, PSTR("%s F"), data);

  x = 64;
  y = 40;
  drawText(data, 3, x, y, center, false);

  display.display();
}

void RenderPercentRh(){
  display.clearDisplay();
  int x, y;
  x = y = 10;
  drawText_P(PSTR("%RH"), 1, x, y, left, false);

  //display humidity
  char data[10];
  dtostrf(percentRH.s, 3, 1, data);
  sprintf_P(data, PSTR("%s %%"), data);

  x = 64;
  y = 40;
  drawText(data, 3, x, y, center, false);

  display.display();
}

void RenderTime()
{
  display.clearDisplay();
  int x, y;
  x = y = 10;
  drawText_P(PSTR("Time"), 1, x, y, left, false); y += 20;
  char data[20];
  int hour = dateTime.hour();
  int minute = dateTime.minute();
  int second = dateTime.second();

  char tick = second % 2 == 1 ? ':' : ' ';
  
  if (GetSettings().is24hr)
    sprintf_P(data, PSTR("%02d%c%02d"), hour, tick, minute);
  else
    sprintf_P(data, PSTR("%02d%c%02d %s"), hour > 12 ? hour - 12 : hour, tick, minute, hour > 12 ? "P" : "A");

  x = 64;
  y = 40;
  drawText(data, 3, x, y, center, false);

  display.display();
}

void drawText_P(const char* text, int textSize, int16_t &x, int16_t &y, ALIGN align, bool superscript) 
{
     char buf[strlen_P(text)+1];
     strcpy_P(buf, text);
     drawText(buf, textSize, x, y, align, superscript);
}

void drawText(const char* text, int textSize, int16_t &x, int16_t &y, ALIGN align, bool superscript)
{
  int16_t x1 = 0, y1 = 0;
  uint16_t w = 0, h = 0;
  display.setCursor(0,0);
  display.setTextWrap(false);
  display.setTextColor(WHITE);
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
