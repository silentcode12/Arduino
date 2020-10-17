class Ema
{
  private:
    float a;
    float s;

  public:
    Ema(float alpha, float initialValue);
    void AddSample(float sample);
    float GetValue();
    void Reset(float value);
};
