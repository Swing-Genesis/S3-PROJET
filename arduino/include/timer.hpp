#ifndef TIMER_HPP
#define TIMER_HPP

#include <Arduino.h>

class Timer {
private:
    unsigned long startTime;
    unsigned long endTime;
    bool running;
    bool enabled;

public:
    Timer() : startTime(0), endTime(0), running(false) {}

    void tic() {
        startTime = millis();
        running = true;
    }

    unsigned long toc() const {
        if (running) {
            return millis() - startTime;
        }
        return 0; // Return 0 if tic() wasn't called before
    }

    float tocSeconds() const {
        return toc() / 1000.0f;
    }

    float tocSeconds() {
        return toc();
    }

    bool isRunning() const {
        return running;
    }

    void reset() {
        startTime = 0;
        running = false;
    }
};

#endif // TIMER_HPP
