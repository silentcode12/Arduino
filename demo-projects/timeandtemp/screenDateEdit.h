class ScreenDateEdit : public Screen
{
  private:
    bool IsLeapYear(short year);
    short year;
    byte month;
    byte day;
    byte dateIndex;
  public:
    ScreenDateEdit();
    void Render(const Adafruit_SSD1306* display, const Context* context) override;
    void ProcessCommitAction(const Context* context) override;
    void ProcessUpdateAction(const Context* context) override;
    void OnShow(const Context* context) override;
    bool AllowAutoChannelChange() override;
};
