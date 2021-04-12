#ifndef SHARED_H
#define SHARED_H

#include "Shiny.h"


/*---------------------------------------------------------------------------*/

PROFILE_SHARED_EXTERN(Math);


/*---------------------------------------------------------------------------*/

void DoSomeMath();

void Recursion(int calls_left);

void ExecuteCommand(const char *command);


/*---------------------------------------------------------------------------*/

void millisleep(unsigned int milliseconds);


#endif // SHARED_H
