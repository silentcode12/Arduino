class ScreenSettings : public Screen
{
private:
	int current=0;
	
public:
	void ProcessUpdateAction(const Context* context) override;
	void ProcessCommitAction(const Context* context) override;
	void Render(const Adafruit_SSD1306* display, const Context* context) override;
	void OnShow(const Context* context) override;
	bool AllowAutoChannelChange() override;
};
