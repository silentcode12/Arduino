class ScreenRh : public Screen
{
public:
	void ProcessUpdateAction(const Context* context) override;
	void Render(const Adafruit_SSD1306* display, const Context* context) override;
};
