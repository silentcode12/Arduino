class ScreenTimeEdit : public Screen
{
 private:
    byte hour;
    byte minute;
    byte second;
    byte timeIndex;
 public:
    ScreenTimeEdit();
    void Render(const Adafruit_SSD1306* display, const Context* context) override;
    void ProcessCommitAction(const Context* context) override;
    void ProcessUpdateAction(const Context* context) override;
    void OnShow(const Context* context) override;
    bool AllowAutoChannelChange() override;
};
