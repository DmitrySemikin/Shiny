#ifndef SHINY_COMMONS_H
#define SHINY_COMMONS_H


// This file should be included into all Shiny sources and headers (except config_cmake.h) as a first line.


//======================================================================================================================

/* Include the configuration knobs: these must be included first
** as they may affect the behaviour of the following configure files
**/
//#include "config_knobs.h"

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


//======================================================================================================================


/* Include the platform checks from CMake */
#include "config/config_cmake.h"



//======================================================================================================================

// TODO: Use defines (or typedefs) to rename standard types and use aliases in code
# include <inttypes.h>

//======================================================================================================================

/* Include the library defines */
//#include "config_define.h"


/*---------------------------------------------------------------------------*/
/*																																					 */
/*  Constants. Usually defined by the platform, but otherwise here					 */
/*																																					 */
/*---------------------------------------------------------------------------*/

#ifndef FALSE
#define FALSE	0x0
#endif

#ifndef TRUE
#define TRUE	0x1
#endif

#ifndef NULL
#define NULL	0
#endif

/*---------------------------------------------------------------------------*/
/*																																					 */
/*  Additional Defines					 																						 */
/*																																					 */
/*---------------------------------------------------------------------------*/

#if SHINY_STATIC_LINK == TRUE
#	define SHINY_API
#else
#	define SHINY_API	SHINY_EXPORT
#endif

/*---------------------------------------------------------------------------*/
/*																																					 */
/*  Structures used by Shiny					 																			 */
/*																																					 */
/*---------------------------------------------------------------------------*/

#if SHINY_IS_COMPILED == TRUE
struct _ShinyNode;
struct _ShinyZone;
#endif

/*---------------------------------------------------------------------------*/
/*																																					 */
/*  Typedefs used by Shiny					 																			   */
/*																																					 */
/*---------------------------------------------------------------------------*/

#if SHINY_IS_COMPILED == TRUE
typedef struct _ShinyNode* ShinyNodeCache;
typedef struct _ShinyNode* ShinyNodeTable;
#endif

#if PLATFORM_TYPE == PLATFORM_TYPE_WINDOWS
/* Use the configured symbols in place of the standard ones, but set a
 * typedef so we don't have to care
 */
typedef INT32_T		int32_t;
typedef UINT32_T	uint32_t;

typedef INT64_T		int64_t;
typedef UINT64_T	uint64_t;
#endif

typedef uint64_t			shinytick_t;



#define SHINY_COMPILER_MSVC 123
#define SHINY_COMPILER_OTHER 124

#ifdef _MSC_VER
#define SHINY_COMPILER SHINY_COMPILER_MSVC
#else
#define SHINY_COMPILER SHINY_COMPILER_OTHER
#endif




//======================================================================================================================

#endif /* SHINY_COMMONS_H */
