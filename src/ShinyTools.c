// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#include "ShinyTools.h"


#if PLATFORM_TYPE == PLATFORM_TYPE_POSIX
#include <sys/time.h>
#endif /* PLATFORM_TYPE_POSIX */

// TODO: Probably we need to include "profileapi.h" instead of windows.h
#if PLATFORM_TYPE == PLATFORM_TYPE_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif



/*---------------------------------------------------------------------------*/

const ShinyTimeUnit* ShinyGetTimeUnit(float ticks) {
    static ShinyTimeUnit units[4] = { 0 };

    if (units[0].tickFreq == 0) { /* auto initialize first time */
        units[0].tickFreq = ShinyGetTickFreq() / 1.0f;
        units[0].invTickFreq = ShinyGetTickInvFreq() * 1.0f;
        units[0].suffix = "s";

        units[1].tickFreq = ShinyGetTickFreq() / 1000.0f;
        units[1].invTickFreq = ShinyGetTickInvFreq() * 1000.0f;
        units[1].suffix = "ms";

        units[2].tickFreq = ShinyGetTickFreq() / 1000000.0f;
        units[2].invTickFreq = ShinyGetTickInvFreq() * 1000000.0f;
        units[2].suffix = "us";

        units[3].tickFreq = ShinyGetTickFreq() / 1000000000.0f;
        units[3].invTickFreq = ShinyGetTickInvFreq() * 1000000000.0f;
        units[3].suffix = "ns";
    }

    if (units[0].tickFreq < ticks) return &units[0];
    else if (units[1].tickFreq < ticks) return &units[1];
    else if (units[2].tickFreq < ticks) return &units[2];
    else return &units[3];
}


/*---------------------------------------------------------------------------*/

#if PLATFORM_TYPE == PLATFORM_TYPE_WINDOWS

void ShinyGetTicks(shinytick_t *p) {
    QueryPerformanceCounter((LARGE_INTEGER*)(p));
}

shinytick_t ShinyGetTickFreq(void) {
    static shinytick_t freq = 0;
    if (freq == 0) QueryPerformanceFrequency((LARGE_INTEGER*)(&freq));
    return freq;
}

float ShinyGetTickInvFreq(void) {
    static float invfreq = 0;
    if (invfreq == 0) invfreq = 1.0f / ShinyGetTickFreq();
    return invfreq;
}

#endif /* PLATFORM_TYPE_WINDOWS */

/*---------------------------------------------------------------------------*/

#if PLATFORM_TYPE == PLATFORM_TYPE_POSIX

void ShinyGetTicks(shinytick_t *p) {
    struct timeval time;
    gettimeofday(&time, NULL);

    *p = time.tv_sec * 1000000 + time.tv_usec;
}

shinytick_t ShinyGetTickFreq(void) {
    return 1000000;
}

float ShinyGetTickInvFreq(void) {
    return 1.0f / 1000000.0f;
}

#endif /* PLATFORM_TYPE_POSIX */


#endif /* SHINY_IS_COMPILED */
