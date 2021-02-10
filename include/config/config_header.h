#ifndef CONFIG_HEADER_H
#define CONFIG_HEADER_H

/**
*** Platform Includes: Core header files required by certain platforms
**/

#if PLATFORM_TYPE == PLATFORM_TYPE_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/**
*** Header Includes: these are conditional, based on the results of the
**  								 header checks above
**/

/* Look for the C99 integer types */
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#else
# error "Cannot build Shiny on this platform: The C99 integer types are undefined"
#endif

/* Look for the header files defining the constants for type lengths */
#ifdef HAVE_VALUES_H
# include <values.h>
#endif

#ifdef HAVE_FLOAT_H
# include <float.h>
#endif

#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif

#ifdef EXPLICIT_MALLOC_H
# include <malloc.h>
#else
# include <stdlib.h>
#endif

#endif /* CONFIG_HEADER_H */
