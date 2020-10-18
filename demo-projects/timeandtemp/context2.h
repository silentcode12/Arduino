class Screen;

class Context2 : Context
{
  public:
    Context2(const RTC_DS3231* rtc, const BME280* bme280, const Adafruit_SSD1306* display, const void (*playAnimationCallback)());
    ~Context2();
    
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
    virtual void RefreshData() override;
    virtual void Begin() override;
    virtual void UpdateInput() override;
    virtual void CommitInput() override;
    virtual void RefreshDisplay() override;

    //Data methods
    virtual void GetDate(short& year, byte& month, byte& day) override;
    virtual void SetDate(const short& year, const byte& month, const byte& day) override;
    virtual void GetTime(byte& hour, byte& minute, byte& second) override;
    virtual void SetTime(const byte& hour, const byte& minute, const byte& second) override;
    virtual const char* GetDayOfWeek() override;

    virtual float GetPercentRh() override;
  
    virtual float GetTemperature() override;
    
    //Navigation methods
    virtual void GotoDateScreen() override;
    virtual void GotoTimeScreen() override;
    virtual void GotoTimeEditScreen() override;
    virtual void GotoDateEditScreen() override;
    virtual void GotoRhScreen() override;
    virtual void GotoTempScreen() override;
    virtual void GotoSettingsScreen() override;
  
    //Settings methods
    virtual bool IsMetric() override;
    virtual void SetIsMetric(bool value) override;

    virtual bool Is24Hour() override;
    virtual void SetIs24Hour(bool value) override;

    virtual bool IsAutoChannelChange() override;
    virtual void SetIsAutoChannelChange(bool value) override;
};
