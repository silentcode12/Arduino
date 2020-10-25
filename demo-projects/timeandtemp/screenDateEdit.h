enum dateEditState
{
  editYearThousands = 0,
  editYearHundreds = 1,
  editYearTens = 2,
  editYearOnes = 3,
  editMonth = 4,
  editDay = 5
};

class ScreenDateEdit : public Screen
{
private:
	bool IsLeapYear(short year);
	short year;
	byte month;
	byte day;
	dateEditState editState;
public:
	ScreenDateEdit();
	void ProcessUpdateAction(const Context* context) override;
	void ProcessCommitAction(const Context* context) override;
	void Render(const Context* context) override;
	void OnShow(const Context* context) override;
	bool AllowAutoChannelChange() override;
};
