// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <SparkFunBME280.h>

#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);
RTC_DS3231 rtc;
BME280 bme280;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
  if (! rtc.begin()) {
    while (1);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
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
}

void loop () {
  DateTime now = rtc.now();

	display.clearDisplay();
  
  display.setTextSize(3);
	display.setTextColor(WHITE);
	display.setCursor(0,0);

  display.print("");
  printNumber(now.hour());
  display.print(':');
  printNumber(now.minute());
  //display.print(':');
  display.setTextSize(1);
  printNumber(now.second());
  display.println();
  display.println();
  display.println();

  display.print(daysOfTheWeek[now.dayOfTheWeek()]);

  display.print(" ");
  display.print(now.year(), DEC);
  display.print('/');
  printNumber(now.month());
  display.print('/');
  printNumber(now.day());
  display.println();

  display.setTextSize(2);
  //display.print(bme280.readTempC(), 2);
  //display.print("C, ");

  display.print(bme280.readTempF(), 2);
  display.setTextSize(1);
  display.print("  o");
  display.setTextSize(2);
  display.println("F");

  display.print(bme280.readFloatHumidity(), 2);
  display.println(" %RH");
  
  //display.print(bme280.readFloatPressure(), 2);
  //display.println(" Pa");

  //display.print(bme280.readFloatAltitudeMeters(), 2);
  //display.print("m, ");

  //display.print(bme280.readFloatAltitudeFeet(), 2);
  //display.println("ft"); 

  display.display();
  delay(125);
}

void printNumber(int number)
{
  if (number < 10) display.print("0");
  display.print(number, DEC);
}
