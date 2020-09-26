class ScreenDateEdit : public Screen
{
  private:
    int dateIndex;
    short date[3] = {0, 0, 0};
  public:
    ScreenDateEdit();
    void Render(const Adafruit_SSD1306* display, const Context* context) override;
    void ProcessCommitAction(const Context* context) override;
    void ScreenDateEdit::ProcessUpdateAction(const Context* context) override;
};
