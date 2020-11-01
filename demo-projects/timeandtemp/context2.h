class Screen;

class Context2 : Context
{
public:
	Context2(const RTC_DS3231* rtc, const BME280* bme280, const Adafruit_SSD1306* display);
	~Context2();
	
private:
	RTC_DS3231* rtc;
	BME280* bme280;
	Adafruit_SSD1306* display;
	Screen* currentScreen;
	void PlayAnimation();
	
	DateTime dateTime;
	
	//BME280 readings
	Ema percentRH;
	Ema temperature;

	void SwapScreen(const Screen* newScreen);

	void SetSetting(byte settingMask, bool value);
	bool GetSetting(byte settingMask);
	
public:
	void RefreshData(bool isUserInput);
  void RefreshDisplay();

  //Initializes the object
	void Begin();

  //User input, update current value
	void UserInputUpdate();
 
  //User input, commit the current value
	void UserInputCommit();
 
	//Data methods
	virtual void GetDate(short& year, byte& month, byte& day, byte& dayOfWeek) override;
	virtual void SetDate(const short& year, const byte& month, const byte& day) override;
 
	virtual void GetTime(byte& hour, byte& minute, byte& second) override;
	virtual void SetTime(const byte& hour, const byte& minute, const byte& second) override;
  
	virtual const char* GetDayString_P(byte dayIndex) override;
  virtual const char* GetMonthString_P(byte monthIndex) override;

	virtual float GetPercentRh() override;

	virtual float GetTemperatureF() override;
	
	//Navigation methods
	virtual void GotoDateScreen() override;
	virtual void GotoTimeScreen() override;
	virtual void GotoTimeEditScreen() override;
	virtual void GotoDateEditScreen() override;
	virtual void GotoRhScreen() override;
	virtual void GotoTempScreen() override;
	virtual void GotoSettingsScreen() override;

	//Settings methods
	virtual bool GetIsMetric() override;
	virtual void SetIsMetric(bool value) override;

	virtual bool GetIs24Hour() override;
	virtual void SetIs24Hour(bool value) override;

	virtual bool GetIsAutoChannelChange() override;
	virtual void SetIsAutoChannelChange(bool value) override;

  //paint methods
  virtual void drawText_P(const char* text, int textSize, int16_t x, int16_t y, ALIGN align, bool superscript) override;
  virtual void drawText(const char* text, int textSize, int16_t x, int16_t y, ALIGN align, bool superscript) override; 
  virtual void drawLine(int16_t x, int16_t y, int16_t x1, int16_t y2, int16_t color) override;
  virtual void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, int16_t color) override;
};
