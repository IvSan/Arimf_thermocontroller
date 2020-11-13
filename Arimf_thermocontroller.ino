#include "max6675.h"
#include "GyverButton.h"
#include "GyverTimer.h"
#include "State.h"
#include "Screen.h"
#include "ProgressTimer.h"
#include "Hysteresis.h"
#include "Pulser.h"
#include "Programmer.h"

//SETTINGS
int PIN_BUTTON_BACKWARD = 5;
int PIN_BUTTON_FORWARD = 6;
int PIN_RELAY = 8;

unsigned long eeprom_save_timeout = 600000;                                  //Периодичность сохранения в eeprom, ms
unsigned long progress_timer_change_step = 600000;                           //Шаг изменения таймера, ms
unsigned int temprtr_target_change_step = 5;                                 //Шаг изменения температуры

//DEVICES
GButton backward_button(PIN_BUTTON_BACKWARD);
GButton forward_button(PIN_BUTTON_FORWARD);
Screen lcd; // А4(sda) А5(scl)
MAX6675 thermocouple(4, 3, 2); // D2 (DO/SO), D3 (CS), D4 (CLK/SCK)

//GLOBAL_VARS
State state;
unsigned long total_ms;
ProgressTimer progress_timer;
GTimer state_save_timer;
GTimer temprtr_read_timer;
int temprtr_actual;
Hysteresis hysteresis;
Pulser pulser;
Programmer programmer;

void setup() {
  Serial.begin(9600);
  Serial.println("==================");
  Serial.println("Start up");

  pinMode(PIN_BUTTON_BACKWARD, INPUT_PULLUP);
  pinMode(PIN_BUTTON_FORWARD, INPUT_PULLUP);
  pinMode(PIN_RELAY, OUTPUT);

  backward_button.setStepTimeout(100);
  forward_button.setStepTimeout(100);

  lcd.setup();

  total_ms = programmer.get_total_ms();
  progress_timer.set_total_ms(total_ms);
  
  backward_button.tick();
  state = State::load(backward_button.state());
  progress_timer.set_progress_minute_dozens(state.progress_minute_dozens);
  hysteresis.set_temprtr_target(state.temprtr_target);

  temprtr_read_timer.setInterval(1000);
  state_save_timer.setInterval(eeprom_save_timeout);

  Serial.println("Setup complete");
}

void loop() {
  forward_button.tick();
  backward_button.tick();

  if (temprtr_read_timer.isReady()) temprtr_actual = thermocouple.readCelsius();
  if (state_save_timer.isReady() && !(state.mode == 2 && state.last_run_success == 1)) state.save();

  switch (state.mode) {
    case 1:
      thermostat();
      break;
    case 2:
      programm();
      break;
    case 0:
    default:
      ask_for_mode();
      break;
  }
}

void ask_for_mode() {
  lcd.ask_for_mode();
  if (backward_button.isPress()) {
    lcd.wipe();
    state.mode = 1;
    Serial.println("Switching to Thermostat mode");
  }
  if (forward_button.isPress()) {
    lcd.wipe();
    progress_timer.resume();
    state.mode = 2;
    Serial.println("Switching to Thermocontroller mode");
  }
}

void thermostat() {
  lcd.thermostat(temprtr_actual, state.temprtr_target);

  if (hysteresis.is_temprtr_low(temprtr_actual) && pulser.is_time_to_heat(state.temprtr_target)) {
    digitalWrite(PIN_RELAY, HIGH);
  } else {
    digitalWrite(PIN_RELAY, LOW);
  }

  if (backward_button.isPress())
    state.temprtr_target -= temprtr_target_change_step;
  else if (backward_button.isStep())
    state.temprtr_target -= 4 * temprtr_target_change_step;

  if (forward_button.isPress())
    state.temprtr_target += temprtr_target_change_step;
  else if (forward_button.isStep())
    state.temprtr_target += 4 * temprtr_target_change_step;

  if (state.temprtr_target < 0) state.temprtr_target = 0;
  if (state.temprtr_target > 999) state.temprtr_target = 999;
  hysteresis.set_temprtr_target(state.temprtr_target);
}

void programm() { 
  progress_timer.resume();
  
  unsigned long progress_ms = progress_timer.get_progress_ms();
  int temprtr_target = programmer.fetch_temp(progress_ms);
  lcd.programm(temprtr_actual, temprtr_target, progress_ms, total_ms, programmer.fetch_name(progress_ms));

  hysteresis.set_temprtr_target(temprtr_target);
  if (hysteresis.is_temprtr_low(temprtr_actual) && pulser.is_time_to_heat(temprtr_target)) {
    digitalWrite(PIN_RELAY, HIGH);
  } else {
    digitalWrite(PIN_RELAY, LOW);
  }

  if (state.last_run_success) {
    return;
  }

  if (state.last_run_success == 0 && progress_timer.is_programm_done()) {
    state.last_run_success = 1;
    state.progress_minute_dozens = progress_timer.get_progress_minute_dozens();
    state.save();
    Serial.println("Program finished");
    return;
  }

  if (backward_button.isPress() || backward_button.isStep())
    progress_timer.add(-progress_timer_change_step);

  if (forward_button.isPress() || forward_button.isStep())
    progress_timer.add(progress_timer_change_step);

  state.progress_minute_dozens = progress_timer.get_progress_minute_dozens();
}
