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
    void (*playAnimationCallback)();
    
    DateTime dateTime;
    
    //BME280 readings
    Ema percentRH;
    Ema temperature;

    void SwapScreen(const Screen* newScreen);

    void SetSetting(byte settingMask, bool value);
    bool GetSetting(byte settingMask);
    
  public:
    void RefreshData();
    void Begin();
	  void UpdateInput();
	  void CommitInput();
    void RefreshDisplay();

	  //Data methods
    void GetDate(short& year, byte& month, byte& day);
    void SetDate(const short& year, const byte& month, const byte& day);
    void GetTime(byte& hour, byte& minute, byte& second);
    void SetTime(const byte& hour, const byte& minute, const byte& second);
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
