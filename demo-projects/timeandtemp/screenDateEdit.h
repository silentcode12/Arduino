class ScreenDateEdit : public Screen
{
  private:
    byte dateIndex;
    bool IsLeapYear(short year);
    short year;
    short month;
    short day;
  public:
    ScreenDateEdit();
    void Render(const Adafruit_SSD1306* display, const Context* context) override;
    void ProcessCommitAction(const Context* context) override;
    void ProcessUpdateAction(const Context* context) override;
    void OnShow(const Context* context) override;
    bool AllowAutoChannelChange() override;
};
