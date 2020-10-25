enum menuItem
{
  setTime = 0,
  setDate = 1,
  setClock = 2,
  setUnits = 3,
  setAutoChannelChange = 4,
  setExit = 5
};

class ScreenSettings : public Screen
{
private:
	menuItem activeMenuItem = setTime;
	
public:
	void ProcessUpdateAction(const Context* context) override;
	void ProcessCommitAction(const Context* context) override;
	void Render(const Context* context) override;
	void OnShow(const Context* context) override;
	bool AllowAutoChannelChange() override;
};
