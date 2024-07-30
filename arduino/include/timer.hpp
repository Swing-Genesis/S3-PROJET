#ifndef TIMER_HPP
#define TIMER_HPP

#include <Arduino.h>

class Timer {
public:
    Timer();
    void setDelay(unsigned long delay);
    void setCallback(void (*callback)());
    void enable();
    void disable();
    void update();
    unsigned long toc();  // Method to get elapsed time

private:
    unsigned long delay_;
    unsigned long lastMillis_;
    void (*callback_)();
    bool enabled_;
    unsigned long startMillis_;  // To track start time
};

// Implementation of Timer methods

Timer::Timer() : delay_(0), lastMillis_(0), callback_(nullptr), enabled_(false), startMillis_(0) {}

void Timer::setDelay(unsigned long delay) {
    delay_ = delay;
}

void Timer::setCallback(void (*callback)()) {
    callback_ = callback;
}

void Timer::enable() {
    enabled_ = true;
    lastMillis_ = millis();
    startMillis_ = millis();
}

void Timer::disable() {
    enabled_ = false;
}

void Timer::update() {
    if (enabled_ && (millis() - lastMillis_ >= delay_)) {
        lastMillis_ = millis();
        if (callback_) {
            callback_();
        }
    }
}

unsigned long Timer::toc() {
    return millis() - startMillis_;
}

#endif // TIMER_HPP
