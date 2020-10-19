class ScreenDateEdit : public Screen
{
private:
	bool IsLeapYear(short year);
	short year;
	byte month;
	byte day;
	byte dateIndex;
public:
	ScreenDateEdit();
	void ProcessUpdateAction(const Context* context) override;
	void ProcessCommitAction(const Context* context) override;
	void Render(const Context* context) override;
	void OnShow(const Context* context) override;
	bool AllowAutoChannelChange() override;
};
