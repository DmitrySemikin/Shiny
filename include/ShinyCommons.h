#ifndef SHINY_COMMONS_H
#define SHINY_COMMONS_H


// This file should be included into all Shiny sources and headers as a first line.


//======================================================================================================================

// TODO: What means "TRUE" and "FALSE" below???

/** Use -DSHINY_IS_COMPILED=0 to compile out all shiny sources from your project. */
#ifndef SHINY_IS_COMPILED
#define SHINY_IS_COMPILED		TRUE
#endif

/** Use -DSHINY_STATIC_LINK=0 to build shiny as a dynamic library (instead of satic). */
#ifndef SHINY_STATIC_LINK
#define SHINY_STATIC_LINK		TRUE
#endif

/** if SHINY_LOOKUP_RATE is defined to TRUE (=1) then Shiny will record the success of its hash function. This is useful for debugging. Default is FALSE. */
#ifndef SHINY_LOOKUP_RATE
#define SHINY_LOOKUP_RATE		FALSE
#endif

/** if SHINY_HAS_ENABLED is defined to TRUE then Shiny can be enabled and disabled at runtime (but it has some overhead). */
#ifndef SHINY_HAS_ENABLED
#define SHINY_HAS_ENABLED		FALSE
#endif

#define SHINY_OUTPUT_MODE_FLAT	0x1
#define SHINY_OUTPUT_MODE_TREE	0x2
#define SHINY_OUTPUT_MODE_BOTH	0x3

#ifndef SHINY_OUTPUT_MODE
#define SHINY_OUTPUT_MODE		SHINY_OUTPUT_MODE_BOTH
#endif


//======================================================================================================================

// TODO: Use defines (or typedefs) to rename standard types and use aliases in code
# include <inttypes.h>

//======================================================================================================================

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

#define PLATFORM_TYPE_WINDOWS	0x01
#define PLATFORM_TYPE_POSIX 	0x02

// TODO: Check do we really need it? Do we need to count MinGW and Cygwin as Windows or Posix?
#if defined(_MSC_VER)
    #define PLATFORM_TYPE PLATFORM_TYPE_WINDOWS
#else
    #define PLATFORM_TYPE PLATFORM_TYPE_POSIX
#endif

#if defined(_MSC_VER)
    #define SHINY_INLINE inline
    #define SHINY_UNUSED
    // TODO: Don't we also need here "dllimport"?
    #define SHINY_EXPORT __declspec(dllexport)
#else
    // assume GCC or Clang or something compatible
    // TODO: "static" is not needed for older versions... but probably it does not harm
    #define SHINY_INLINE static __inline__
    #define SHINY_UNUSED __attribute__((unused))
    // TODO: Do we really need dll export for GCC? For MinGW? Don't we also need "dllimport" then?
    #define SHINY_EXPORT __attribute__((dllexport))
#endif // _MSC_VER


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

// TODO: Fix this on windows, if needed.
// TODO: Replace direct use of int32_t, uint32_t with shiny_int32_t etc.

//if ( CYGWIN )
//    set ( UINT32_T  "u_int32_t" )
//    set ( UINT64_T  "u_int64_t" )
//else ( CYGWIN )
//    set ( INT32_T   "int"                 )
//    set ( UINT32_T  "unsigned int"        )
//    set ( INT64_T   "__int64"             )
//    set ( UINT64_T  "unsigned __int64"    )
//endif ( CYGWIN )

///* Use the configured symbols in place of the standard ones, but set a
// * typedef so we don't have to care
// */
//typedef INT32_T		int32_t;
//typedef UINT32_T	uint32_t;
//
//typedef INT64_T		int64_t;
//typedef UINT64_T	uint64_t;
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
