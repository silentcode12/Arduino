enum timeEditState
{
  editHour = 0,
  editMinute = 1,
  editSecond = 2
};

class ScreenTimeEdit : public Screen
{
private:
	byte hour;
	byte minute;
	byte second;
	timeEditState editState;
public:
	ScreenTimeEdit();
	void ProcessUpdateAction(const Context* context) override;
	void ProcessCommitAction(const Context* context) override;
	void Render(const Context* context) override;
	void OnShow(const Context* context) override;
	bool AllowAutoChannelChange() override;
	void Tick() override;
};
