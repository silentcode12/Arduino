class Context;

class Screen 
{
  public:
    Screen();
  
  public:
    virtual void Render(const Adafruit_SSD1306* display, const Context* context) = 0;
    virtual void ProcessCommitAction(const Context* context);
    virtual void ProcessUpdateAction(const Context* context) = 0;
    virtual void OnShow(const Context* context);

  protected:
    void drawText_P(const Adafruit_SSD1306* display, const char* text, int textSize, int16_t &x, int16_t &y, ALIGN align, bool superscript);
    void drawText(const Adafruit_SSD1306* display, const char* text, int textSize, int16_t &x, int16_t &y, ALIGN align, bool superscript); 
};
