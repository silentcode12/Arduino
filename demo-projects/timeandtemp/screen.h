//For more details, look up the design pattern called "State Pattern"
class Context;

class Screen 
{
public:
  virtual ~Screen();
  
public:
	virtual void ProcessUpdateAction(const Context* context);
	virtual void ProcessCommitAction(const Context* context);
	virtual void Render(const Context* context);
	virtual void OnShow(const Context* context);
	virtual bool AllowAutoChannelChange();
	virtual void Tick();
};
