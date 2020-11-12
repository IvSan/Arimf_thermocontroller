#include <LCD_1602_RUS.h>

class Screen {
  public:
    void setup();
    void wipe();
    void ask_for_mode();
    void thermostat(unsigned int temprtr_actual, unsigned int temprtr_target);
    void programm(unsigned int temprtr_actual, int temprtr_target, unsigned long progress_ms, unsigned long total_lengths, String current_step);
  private:
    LCD_1602_RUS lcd {0x27, 16, 2};
    boolean wipe_next_time = false;
    byte last_step_name_length;
    void show_temprtr_taget(unsigned int temprtr_taget);
    void show_temprtr_actual(unsigned int temprtr_actual);
};

void Screen::setup() {
  lcd.begin();
  lcd.backlight();
}

void Screen::wipe() {
  wipe_next_time = true;
}

void Screen::ask_for_mode() {
  if (wipe_next_time) {
    lcd.clear();
    wipe_next_time = false;
  }
  lcd.setCursor(0, 0); lcd.print(L"< Термостат");
  lcd.setCursor(0, 1); lcd.print(L"> Программа");
}

void Screen::thermostat(unsigned int temprtr_actual, unsigned int temprtr_target) {
  if (wipe_next_time) {
    lcd.clear();
    wipe_next_time = false;
  }

  lcd.setCursor(3, 0); lcd.print(L"Установка");
  lcd.setCursor(1, 1); lcd.print(L"Температура");

  show_temprtr_taget(temprtr_target);
  show_temprtr_actual(temprtr_actual);
}

void Screen::programm(unsigned int temprtr_actual, int temprtr_target, unsigned long progress_ms, unsigned long total_length, String current_step) {
  if (wipe_next_time || last_step_name_length != current_step.length()) {
    lcd.clear();
    wipe_next_time = false;
    last_step_name_length = current_step.length();
  }

  long remain_ms = total_length - progress_ms;

  if (remain_ms < 0) {
    remain_ms = 0;
  }

  unsigned int remain_seconds = remain_ms / 1000;
  unsigned int remain_minutes = remain_seconds / 60;
  unsigned int remain_hours = remain_minutes / 60;

  unsigned int remain_minutes_leftover = (remain_seconds / 60) - (remain_hours * 60);
  unsigned int remain_seconds_leftover = remain_seconds - (remain_minutes_leftover * 60) - (remain_hours * 60 * 60);

  lcd.setCursor(0, 0); lcd.print(current_step);

  lcd.setCursor(0, 1);
  lcd.print(remain_hours);
  lcd.setCursor(1, 1);
  lcd.print(".");

  if (remain_minutes_leftover < 10) {
    lcd.print("0");
    lcd.print(remain_minutes_leftover);
  } else lcd.print(remain_minutes_leftover);

  lcd.print(".");

  if (remain_seconds_leftover < 10) {
    lcd.print("0");
    lcd.print(remain_seconds_leftover);
  }
  else lcd.print(remain_seconds_leftover);

  show_temprtr_taget(temprtr_target);
  show_temprtr_actual(temprtr_actual);
}

void Screen::show_temprtr_taget(unsigned int temprtr_target) {
  lcd.setCursor(13, 0);
  lcd.print(temprtr_target);
  if (temprtr_target < 100) {
    lcd.print("  ");
  }
}

void Screen::show_temprtr_actual(unsigned int temprtr_actual) {
  lcd.setCursor(13, 1);
  lcd.print(temprtr_actual);
  if (temprtr_actual < 100) {
    lcd.print("  ");
  }
}
