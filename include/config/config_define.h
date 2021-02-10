#ifndef CONFIG_DEFINE_H
#define CONFIG_DEFINE_H

/**
*** \note This file should be kept as small as possible: if it doesn't depend
***       on the platform, it probably doesn't belong in here.
***
***/

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

#endif /* CONFIG_DEFINE_H */
