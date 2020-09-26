class ScreenTemp : public Screen
{
  public:
    void Render(const Adafruit_SSD1306* display, const Context* context) override;
    void ProcessCommitAction(const Context* context) override;
    void ProcessUpdateAction(const Context* context) override;
};
