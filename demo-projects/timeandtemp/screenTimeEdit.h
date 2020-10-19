class ScreenTimeEdit : public Screen
{
private:
	byte hour;
	byte minute;
	byte second;
	byte timeIndex;
public:
	ScreenTimeEdit();
	void ProcessUpdateAction(const Context* context) override;
	void ProcessCommitAction(const Context* context) override;
	void Render(const Context* context) override;
	void OnShow(const Context* context) override;
	bool AllowAutoChannelChange() override;
	void Tick() override;
};
