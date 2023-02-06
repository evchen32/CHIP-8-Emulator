#include "Timer.h"

class Timer {

    public:
        Timer::Timer() {
            startTicks = 0;
        }

        // Starts the timer
        void Timer::start() {
            startTicks = SDL_GetTicks();

        }

        // Gets the time elapsed since timer start
        uint32_t Timer::getTicks() {
            return SDL_GetTicks() - startTicks;
        }
};