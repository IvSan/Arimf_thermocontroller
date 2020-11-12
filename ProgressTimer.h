#include <Chrono.h>

class ProgressTimer {
  public:
    unsigned long get_progress_ms();
    void set_progress_ms(unsigned long progress_ms);
    byte get_progress_minute_dozens();
    void set_progress_minute_dozens(byte progress_minute_dozens);
    void set_total_ms(unsigned long total_ms);
    void resume();
    void stop();
    void add(long offset);
    boolean is_programm_done();
  private:
    unsigned long progress_ms;
    unsigned long total_ms;
    Chrono chrono{Chrono::MILLIS, false};
    void resetChrono();
};

unsigned long ProgressTimer::get_progress_ms() {
  return progress_ms + chrono.elapsed();
}

void ProgressTimer::set_progress_ms(unsigned long progress_ms) {
  this->progress_ms = progress_ms;
  resetChrono();
}

byte ProgressTimer::get_progress_minute_dozens() {
  return get_progress_ms() / 600000;
}

void ProgressTimer::set_progress_minute_dozens(byte progress_minute_dozens) {
  progress_ms = progress_minute_dozens * 600000;
}

void ProgressTimer::set_total_ms(unsigned long total_ms) {
  this->total_ms = total_ms;
}

void ProgressTimer::resume() {
  chrono.resume();
}

void ProgressTimer::stop() {
  chrono.stop();
}

void ProgressTimer::add(long offset) {
  chrono.add(offset);
}

boolean ProgressTimer::is_programm_done() {
  return get_progress_ms() >= total_ms;
}

void ProgressTimer::resetChrono() {
  if (chrono.isRunning()) {
    chrono.restart();
  } else {
    chrono.restart();
    chrono.stop();
  }
}
