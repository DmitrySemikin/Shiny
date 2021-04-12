#include "Shiny.h"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h> // Sleep
#else // assume POSIX
    #include <unistd.h> // usleep
#endif

#include "Shared.h"


/*---------------------------------------------------------------------------*/

void millisleep(unsigned int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}


/*---------------------------------------------------------------------------*/

void YetMoreMath() {
    PROFILE_SHARED_BLOCK(Math); // profile until end of block (only supported in c++)

    // do stuff
}


/*---------------------------------------------------------------------------*/

int main() {

    DoSomeMath();
    YetMoreMath();
    Recursion(12);

#ifdef _WIN32
    ExecuteCommand("cls");
#else
    ExecuteCommand("clear");
#endif

    PROFILE_UPDATE(); // update all profiles
    PROFILE_OUTPUT(NULL); // print to terminal

    return 0;
}
