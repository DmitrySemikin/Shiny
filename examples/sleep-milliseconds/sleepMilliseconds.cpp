#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h> // Sleep
#else // assume POSIX
    #include <unistd.h> // usleep
#endif


#include "sleepMilliseconds.h"


void sleepMilliseconds(unsigned int milliseconds) {

#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif

}
