class Context
{	
public:
	//Data methods
	virtual void GetDate(short& year, byte& month, byte& day) = 0;
	virtual void SetDate(const short& year, const byte& month, const byte& day) = 0;
	
	virtual void GetTime(byte& hour, byte& minute, byte& second) = 0;
	virtual void SetTime(const byte& hour, const byte& minute, const byte& second) = 0;
	
	virtual const char* GetDayOfWeek() = 0;

	virtual float GetPercentRh() = 0;
	
	virtual float GetTemperatureF() = 0;
	
	//Navigation methods
	virtual void GotoDateScreen() = 0;
	virtual void GotoTimeScreen() = 0;
	virtual void GotoTimeEditScreen() = 0;
	virtual void GotoDateEditScreen() = 0;
	virtual void GotoRhScreen() = 0;
	virtual void GotoTempScreen() = 0;
	virtual void GotoSettingsScreen() = 0;
	
	//Settings methods
	virtual bool GetIsMetric() = 0;
	virtual void SetIsMetric(bool value) = 0;

	virtual bool GetIs24Hour() = 0;
	virtual void SetIs24Hour(bool value) = 0;

	virtual bool GetIsAutoChannelChange() = 0;
	virtual void SetIsAutoChannelChange(bool value) = 0;
};
