#include <EEPROMex.h>

#define mode_address 0
#define temprtr_target_address 1
#define last_run_success_address 3
#define progress_minute_dozens_address 4

class State {
  public:
    State(byte mode = 0, byte temprtr_target = 100, byte last_run_success = 0, byte progress_minute_dozens = 0);
    byte mode; // 0 - NOT_SELECTED, 1 - THERMOSTAT, 2 - PROGRAMM
    int temprtr_target = 100;
    byte last_run_success = 0;
    byte progress_minute_dozens = 0;

    static State load(boolean reset = false);
    void save();
    void toSerial(boolean);
};

State::State(byte mode, byte temprtr_target, byte last_run_success, byte progress_minute_dozens) {
  this->mode = mode;
  this->temprtr_target = temprtr_target;
  this->last_run_success = last_run_success;
  this->progress_minute_dozens = progress_minute_dozens;
}

State State::load(boolean reset) {
  State state;

  if (reset) {
    Serial.println("Reset");
    state.save();
    delay(1000);
    return state;
  }

  state.mode = EEPROM.read(mode_address);

  switch (state.mode) {
    case 0:
      break;

    case 1:
      state.temprtr_target = EEPROM.readInt(temprtr_target_address);
      break;

    case 2:
      state.last_run_success = EEPROM.read(last_run_success_address);
      state.progress_minute_dozens = EEPROM.read(progress_minute_dozens_address);
      break;

    default:
      for (int i = 0; i <= 3; i++) {
        EEPROM.write(i, 0);
      }
      state.mode = 0;
      break;
  }
  state.toSerial(false);
  return state;
}

void State::save() {
  EEPROM.write(mode_address, mode);
  switch (mode) {
    case 0:
      break;
    case 1:
      EEPROM.writeInt(temprtr_target_address, temprtr_target);
      break;
    case 2:
    default:
      EEPROM.write(last_run_success_address, last_run_success);
      EEPROM.write(progress_minute_dozens_address, progress_minute_dozens);
      break;
  }
  toSerial(true);
}

void State::toSerial(boolean save) {
  Serial.println(String(save ? "State save" : "State load")
                 + ", mode=" + mode
                 + ", temprtr_target=" + temprtr_target
                 + ", last_run_success=" + last_run_success
                 + ", progress_minute_dozens=" + progress_minute_dozens);
}
