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

    DateTime dateTime;
    
    Screen* currentScreen;
    
    //BME280 readings
    Ema percentRH;
    Ema temperature;
    //Ema altitude;
    //Ema pressure;

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
    bool IsMetric();
    void SetIsMetric(bool value);

    bool Is24Hour();
    void SetIs24Hour(bool value);

    bool IsAutoChannelChange();
    void SetIsAutoChannelChange(bool value);
};
