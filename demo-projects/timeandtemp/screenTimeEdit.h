class ScreenTimeEdit : public Screen
{
 private:
    short hour;
    short minute;
    short second;
    int timeIndex;
 public:
    ScreenTimeEdit();
    void Render(const Adafruit_SSD1306* display, const Context* context) override;
    void ProcessCommitAction(const Context* context) override;
    void ProcessUpdateAction(const Context* context) override;
    void OnShow(const Context* context) override;
};