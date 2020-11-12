class Pulser {
  public:
    float heater_power = 0.5;
    boolean is_time_to_heat(int temprtr_target);
  private:
    int heater_cycle = 2000;
    boolean time_to_heat = true;
    long last_time_heater_power_switch;
    float calc_power(int temprtr_target);
};


boolean Pulser::is_time_to_heat(int temprtr_target) {
  heater_power = calc_power(temprtr_target);

  int heater_on_time = heater_cycle * heater_power;
  int heater_off_time = heater_cycle - heater_on_time;
  int current_phase_time = time_to_heat ? heater_on_time : heater_off_time;

  if (millis() - last_time_heater_power_switch > current_phase_time) {  //Когда счетчик переваливает за выбранную длительность импульса
    time_to_heat = !time_to_heat;                                       //Переключаем флаг импульса нагрева
    last_time_heater_power_switch = millis();
  }

  return time_to_heat;
}

float Pulser::calc_power(int temprtr_target) {
  if (temprtr_target <= 100) return 0.15;
  if (temprtr_target <= 200) return 0.2;
  if (temprtr_target <= 300) return 0.3;
  if (temprtr_target <= 400) return 0.5;
  return 1;
}
