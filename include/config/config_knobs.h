#ifndef CONFIG_KNOBS_H
#define CONFIG_KNOBS_H

/**
*** \file config_knobs.h
***
*** \brief This file contains a list of knobs that affect the configuration of Shiny. Use with care!
***
**/

/* SHINY_IS_COMPILED is the master on or off swith at compile time. Define it to TRUE or FALSE before including header Shiny.h or inside ShinyConfig.h. Default is TRUE.
 */
#ifndef SHINY_IS_COMPILED
#define SHINY_IS_COMPILED		TRUE
#endif

/* TODO: 
 */
#ifndef SHINY_STATIC_LINK
#define SHINY_STATIC_LINK		TRUE
#endif

/* if SHINY_LOOKUP_RATE is defined to TRUE then Shiny will record the success of its hash function. This is useful for debugging. Default is FALSE.
 */
#ifndef SHINY_LOOKUP_RATE
#define SHINY_LOOKUP_RATE		FALSE
#endif

/* if SHINY_HAS_ENABLED is defined to TRUE then Shiny can be enabled and disabled at runtime. TODO: bla bla...
 */
#ifndef SHINY_HAS_ENABLED
#define SHINY_HAS_ENABLED		FALSE
#endif

/* TODO: 
 */
#define SHINY_OUTPUT_MODE_FLAT	0x1

/* TODO: 
 */
#define SHINY_OUTPUT_MODE_TREE	0x2

/* TODO: 
 */
#define SHINY_OUTPUT_MODE_BOTH	0x3

/* TODO: 
 */
#ifndef SHINY_OUTPUT_MODE
#define SHINY_OUTPUT_MODE		SHINY_OUTPUT_MODE_BOTH
#endif

#endif /* CONFIG_KNOBS_H */
