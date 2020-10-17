#include "ema.h"

typedef struct _SETTINGS
{
  bool is24hr;
  bool isMetric;
}SETTINGS;

#define TEMP_CORRECTION -10.0

const char daysOfTheWeek[7][4] PROGMEM = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  //Stored in flash, read out using sprintf_P with %S

class Screen;

class Context
{
	public:
		Context(const RTC_DS3231* rtc, const BME280* bme280, const Adafruit_SSD1306* display, const void (*playAnimationCallback)());
		~Context();
		
  private:
    RTC_DS3231* rtc;
    BME280* bme280;
    Adafruit_SSD1306* display;
    
    Screen* currentScreen;
    
    //BME280 readings
    Ema percentRH;
    Ema temperature;
    Ema altitude;
    Ema pressure;

    DateTime dateTime;

    void (*playAnimationCallback)();

    void SwapScreen(const Screen* newScreen);
    
  public:
    void RefreshData();
    void Begin();
	  void UpdateInput();
	  void CommitInput();
    void RefreshDisplay();

	  //Data methods
    void GetDate(short& year, short& month, short& day);
    void SetDate(const short& year, const short& month, const short& day);
    void GetTime(short& hour, short& minute, short& second);
    void SetTime(const short& hour, const short& minute, const short& second);
    const char* GetDayOfWeek();

	  float GetPercentRh();
	
    float GetTemperature();
    
	  //Navigation methods
    void GotoDateScreen();
    void GotoTimeScreen();
    void GotoTimeEditScreen();
    void GotoDateEditScreen();
    void GotoRhScreen();
    void GotoTempScreen();
    void GotoSettingsScreen();
	
	  //Settings methods
    SETTINGS GetSettings();
    void SetSettings(SETTINGS settings);
};
