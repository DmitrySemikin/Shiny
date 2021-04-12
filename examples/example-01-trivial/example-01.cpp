/*
 * This is very basic illustration of using das-shiny.
 *
 * It demonstrates how to define profiling regions and
 * how to print profiling results.
 *
 * Sample output of this application:

sorted list                               calls       self time      total time
<unprofiled>                                1.0      0 ns    0%      6 s   100%
region_3sec                                 1.0      3 s    50%      3 s    50%
block_2sec                                  1.0      2 s    33%      2 s    33%
function_1sec                               1.0      1 s    17%      1 s    17%

call tree                                 calls       self time      total time
<unprofiled>                                1.0      0 ns    0%      6 s   100%
  function_1sec                             1.0      1 s    17%      1 s    17%
  block_2sec                                1.0      2 s    33%      2 s    33%
  region_3sec                               1.0      3 s    50%      3 s    50%

 */

#include <Shiny.h>

#include "sleepMilliseconds.h"

void function_1sec() {
    PROFILE_FUNC(); // Profile from this point to the end of function (named after function).
    sleepMilliseconds(1000);
}


int main() {


    function_1sec(); // Function is profiled - see it's definition


    {
        PROFILE_BLOCK(block_2sec); // (C++ only) Profile until end of the code block.

        sleepMilliseconds(2000);
    }


    PROFILE_BEGIN(region_3sec); // Profile until `PROFILE_END()`

    sleepMilliseconds(3000);

    PROFILE_END();

    // - - - - -

    PROFILE_UPDATE(); // Update counters (must be done before querying profiling values
    PROFILE_OUTPUT(NULL); // Print profiling results to console (or to file).

    return 0;
}
