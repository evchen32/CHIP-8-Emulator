#ifndef TIMER_H
#define TIMER_H

#include <SDL2/SDL.h>

class Timer {
    private:
        uint32_t startTicks;
    
    public:
        Timer();

        // Starts the timer
        void start();

        // Gets the time elapsed since timer start
        uint32_t getTicks();
};

#endif