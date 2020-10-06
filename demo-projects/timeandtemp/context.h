typedef struct _SETTINGS
{
  bool is24hr;
  bool isMetric;
}SETTINGS;

typedef struct 
{
  float a;      //initialization of EMA alpha
  float s;      //initialization of EMA S
} EMA;

const char daysOfTheWeek[7][4] PROGMEM = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  //Stored in flash, read out using sprintf_P with %S

class Screen;

class Context
{
  private:
    //Display screen definitions
    
    Screen* currentScreen;

  public:
    DateTime GetCurrentDateTime();
    void SetDateTime(DateTime& newDateTime);
    
    void GotoDateScreen();
    void GotoTimeScreen();
    void GotoTimeEditScreen();
    void GotoDateEditScreen();
    void GotoRhScreen();
    void GotoTempScreen();

    
    SETTINGS GetSettings();
    void SetSettings(SETTINGS settings);
    EMA GetPercentRh();
    EMA GetTemperature();
    bool IsLeapYear(short year);

    Context();
    ~Context();
    const Screen* GetCurrentScreen(){return currentScreen;}
};
