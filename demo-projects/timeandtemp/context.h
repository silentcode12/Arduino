typedef enum
{
  left,
  center,
  right
} ALIGN;

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

class Context
{
  public:
    DateTime GetCurrentDateTime();
    SETTINGS GetSettings();
    EMA GetPercentRh();
    EMA GetTemperature();
};
