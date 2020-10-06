class ScreenTimeEdit : public Screen
{
 private:
    short time[3] = {0, 0, 0};
    int timeIndex;
 public:
    ScreenTimeEdit();
    void Render(const Adafruit_SSD1306* display, const Context* context) override;
    void ProcessCommitAction(const Context* context) override;
    void ProcessUpdateAction(const Context* context) override;
    void OnShow(const Context* context) override;
};
