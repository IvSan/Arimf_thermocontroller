class Hysteresis {
  public:
    int temprtr_target;
    void set_temprtr_target(int temprtr_target);
    boolean is_temprtr_low(int temprtr_actual);
  private:
    int temprtr_hysteresis = 2;
    boolean temprtr_low = true;
};

void Hysteresis::set_temprtr_target(int temprtr_target) {
  this->temprtr_target = temprtr_target;
}

boolean Hysteresis::is_temprtr_low(int temprtr_actual) {
  if (temprtr_actual < temprtr_target - temprtr_hysteresis)
    temprtr_low = true;
  else if (temprtr_actual > temprtr_target)
    temprtr_low = false;

  return temprtr_low;
}
