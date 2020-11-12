class Programmer {
  public:
    unsigned long get_total_ms();
    int fetch_temp(long time);
    String fetch_name(long time);
};

unsigned long Programmer::get_total_ms() {
  return 540L * 60 * 1000;
}

int Programmer::fetch_temp(long time) {
  int minutes = time / 60000;
  if (minutes < 240) return 120;
  if (minutes < 360) return map(minutes, 240, 360, 170, 350);
  if (minutes < 480) return 700;
  if (minutes < 540) return 500;
  return 0;
}

String Programmer::fetch_name(long time) {
  int minutes = time / 60000;
  if (minutes < 240) return ("Вытопка");
  if (minutes < 360) return ("Разогрев");
  if (minutes < 480) return ("Прокалка");
  if (minutes < 540) return ("Остывание");
  return "Завершено";
}
