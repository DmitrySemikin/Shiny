#include "Shiny.h"

#include "Shared.h"


/*---------------------------------------------------------------------------*/

PROFILE_SHARED_DEFINE(Math);

void DoSomeMath() {
    PROFILE_SHARED_BLOCK(Math); // profile until end of block (only supported in c++)

    // do stuff
}


/*---------------------------------------------------------------------------*/

void Recursion(int calls_left) {
    PROFILE_FUNC(); // begin profile until end of block

    millisleep(20);

    if (calls_left > 0) {
        Recursion(calls_left - 1);
    }
}


/*---------------------------------------------------------------------------*/

void ExecuteCommand(const char *command) {

    PROFILE_CODE(system(command));
}
