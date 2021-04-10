#include "Shiny.h"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h> // Sleep
#else // assume POSIX
    #include <unistd.h> // usleep
#endif


/*---------------------------------------------------------------------------*/

void millisleep(unsigned int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}


/*---------------------------------------------------------------------------*/

void LazyHelloWorld() {
    PROFILE_FUNC(); // profile until end of block (only supported in c++)

    millisleep(100);
}


/*---------------------------------------------------------------------------*/

void HelloWorld() {
    PROFILE_BEGIN(Hello_World__This_is_Shiny); // profile until PROFILE_END()

    millisleep(100);

    PROFILE_END();
}


/*---------------------------------------------------------------------------*/

int main() {

    HelloWorld();

    LazyHelloWorld();

    PROFILE_UPDATE(); // update all profiles
    PROFILE_OUTPUT(NULL); // print to terminal

    return 0;
}
