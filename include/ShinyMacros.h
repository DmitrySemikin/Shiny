#ifndef SHINY_MACROS_H
#define SHINY_MACROS_H

/* ShinyCommons.h must be included into all sources and headers as a first include */
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#include "ShinyManager.h"


/*---------------------------------------------------------------------------*/
/* public preprocessors */

#define PROFILE_UPDATE()													\
    ShinyManager_update(&Shiny_instance)

#define PROFILE_SET_DAMPING(floatfrom0to1)									\
    Shiny_instance.damping = (floatfrom0to1);

#define PROFILE_GET_DAMPING()												\
    (Shiny_instance.damping)

#define PROFILE_OUTPUT(filename)											\
    ShinyManager_output(&Shiny_instance, (filename))

#define PROFILE_OUTPUT_STREAM(stream)										\
    ShinyManager_outputToStream(&Shiny_instance, (stream))

#ifdef __cplusplus
#define PROFILE_GET_TREE_STRING()											\
    ShinyManager_outputTreeToString(&Shiny_instance)

#define PROFILE_GET_FLAT_STRING()											\
    ShinyManager_outputFlatToString(&Shiny_instance)
#endif

#define PROFILE_DESTROY()													\
    ShinyManager_destroy(&Shiny_instance)

#define PROFILE_CLEAR()														\
    ShinyManager_clear(&Shiny_instance)

#define PROFILE_SORT_ZONES()												\
    ShinyManager_sortZones(&Shiny_instance)


/*---------------------------------------------------------------------------*/
/* public preprocessors */

#define PROFILE_GET_TOTAL_TICKS_CUR()										\
    ShinyData_totalTicksCur(&Shiny_instance.rootZone.data)

#define PROFILE_GET_TOTAL_TICKS()											\
    ShinyData_totalTicksAvg(&Shiny_instance.rootZone.data)

#define PROFILE_GET_PROFILED_TICKS_CUR()									\
    (Shiny_instance.rootZone.data.selfTicks.cur)

#define PROFILE_GET_PROFILED_TICKS()										\
    (Shiny_instance.rootZone.data.selfTicks.avg)

#define PROFILE_GET_UNPROFILED_TICKS_CUR()									\
    (Shiny_instance.rootZone.data.childTicks.cur)

#define PROFILE_GET_UNPROFILED_TICKS()										\
    (Shiny_instance.rootZone.data.childTicks.avg)

#define PROFILE_GET_SHARED_TOTAL_TICKS_CUR(name)							\
    ShinyData_totalTicksCur(&(_PROFILE_ID_ZONE_SHARED(name).data))

#define PROFILE_GET_SHARED_TOTAL_TICKS(name)								\
    ShinyData_totalTicksAvg(&(_PROFILE_ID_ZONE_SHARED(name).data))

#define PROFILE_GET_SHARED_SELF_TICKS_CUR(name)								\
    (_PROFILE_ID_ZONE_SHARED(name).data.selfTicks.cur)

#define PROFILE_GET_SHARED_SELF_TICKS(name)									\
    (_PROFILE_ID_ZONE_SHARED(name).data.selfTicks.avg)


/*---------------------------------------------------------------------------*/
/* public preprocessors */

#define PROFILE_IS_SHARED_SELF_BELOW(name, floatfrom0to1)					\
    ShinyManager_isZoneSelfTimeBelow(										\
        &Shiny_instance, _PROFILE_ID_ZONE_SHARED(name), floatfrom0to1)

#define PROFILE_IS_SHARED_TOTAL_BELOW(name, floatfrom0to1)					\
    ShinyManager_isZoneTotalTimeBelow(										\
        &Shiny_instance, _PROFILE_ID_ZONE_SHARED(name), floatfrom0to1)


/*---------------------------------------------------------------------------*/
/* public preprocessors */

#define PROFILE_END()														\
    ShinyManager_endCurNode(&Shiny_instance)


/*---------------------------------------------------------------------------*/
/* public preprocessors */

#define PROFILE_BEGIN( name )												\
                                                                            \
    static _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE(name), #name);				\
    _PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE(name))


/*---------------------------------------------------------------------------*/
/* public preprocessors */

#ifdef __cplusplus
#define PROFILE_BLOCK( name )												\
                                                                            \
    _PROFILE_BLOCK_DEFINE(_PROFILE_ID_BLOCK());								\
    PROFILE_BEGIN(name)
#endif

/*---------------------------------------------------------------------------*/
/* public preprocessors */

#ifdef __cplusplus
#define PROFILE_FUNC()														\
                                                                            \
    _PROFILE_BLOCK_DEFINE(_PROFILE_ID_BLOCK());								\
    static _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_FUNC(), __FUNCTION__);		\
    _PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_FUNC())
#endif

/*---------------------------------------------------------------------------*/
/* public preprocessors */

#define PROFILE_CODE( code )												\
                                                                            \
    do {																	\
        static _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_CODE(), #code);		\
        _PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_CODE());						\
        { code; }															\
        PROFILE_END();														\
    } while(0)


/*---------------------------------------------------------------------------*/
/* public preprocessors */

#define PROFILE_SHARED_EXTERN( name )										\
                                                                            \
    _PROFILE_ZONE_DECLARE(extern, _PROFILE_ID_ZONE_SHARED(name))


/*---------------------------------------------------------------------------*/
/* public preprocessors */

#define PROFILE_SHARED_DEFINE( name )										\
                                                                            \
    _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_SHARED(name), #name)


/*---------------------------------------------------------------------------*/
/* public preprocessors */

#define PROFILE_SHARED_BEGIN( name )										\
                                                                            \
    _PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_SHARED(name))


/*---------------------------------------------------------------------------*/
/* public preprocessors */

#ifdef __cplusplus
#define PROFILE_SHARED_BLOCK( name )										\
                                                                            \
    _PROFILE_BLOCK_DEFINE(_PROFILE_ID_BLOCK());								\
    _PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_SHARED(name))
#endif


/*---------------------------------------------------------------------------*/
/* internal preprocessors */

#define _PROFILE_ID_ZONE( name )			__ShinyZone_##name
#define _PROFILE_ID_ZONE_FUNC()				__ShinyZoneFunc
#define _PROFILE_ID_ZONE_CODE()				__ShinyZoneCode
#define _PROFILE_ID_ZONE_SHARED( name )		name##__ShinyZoneShared
#define _PROFILE_ID_BLOCK()					__ShinyBlock


/*---------------------------------------------------------------------------*/
/* internal preprocessor */

#define _PROFILE_ZONE_DEFINE( id, string )									\
                                                                            \
    ShinyZone id = {														\
        NULL, SHINY_ZONE_STATE_HIDDEN, string,								\
        { { 0, 0 }, { 0, 0 }, { 0, 0 } }									\
    }


/*---------------------------------------------------------------------------*/
/* internal preprocessor */

#define _PROFILE_ZONE_DECLARE( prefix, id )									\
                                                                            \
    prefix ShinyZone id


/*---------------------------------------------------------------------------*/
/* internal preprocessor */

#define _PROFILE_BLOCK_DEFINE( id )											\
                                                                            \
    ShinyEndNodeOnDestruction SHINY_UNUSED id


/*---------------------------------------------------------------------------*/
/* internal preprocessor */

#define _PROFILE_ZONE_BEGIN( id )											\
                                                                            \
    do {																	\
        static ShinyNodeCache cache = &_ShinyNode_dummy;					\
        ShinyManager_lookupAndBeginNode(&Shiny_instance, &cache, &id);		\
    } while(0)

/*---------------------------------------------------------------------------*/


#else /* SHINY_IS_COMPILED */


SHINY_INLINE ShinyData GetEmptyData() {
    ShinyData a = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
    return a;
}

#define PROFILE_UPDATE()
#define PROFILE_CLEAR()
#define PROFILE_DESTROY()

#define PROFILE_SET_DAMPING(x)
#define PROFILE_GET_DAMPING()			0.0f

#define PROFILE_OUTPUT(x)
#define PROFILE_OUTPUT_STREAM(x)
#define PROFILE_GET_TREE_STRING()		std::string()
#define PROFILE_GET_FLAT_STRING()		std::string()

#define PROFILE_BEGIN(name)
#define PROFILE_BLOCK(name)

#define PROFILE_FUNC()
#define PROFILE_CODE(code)				do { code; } while (0)

#define PROFILE_SHARED_GLOBAL(name)
#define PROFILE_SHARED_MEMBER(name)
#define PROFILE_SHARED_DEFINE(name)
#define PROFILE_SHARED_BEGIN(name)
#define PROFILE_SHARED_BLOCK(name)

#define PROFILE_GET_SHARED_DATA(name)	ShinyGetEmptyData()
#define PROFILE_GET_ROOT_DATA()			ShinyGetEmptyData()


#endif /* SHINY_IS_COMPILED */


#endif /* SHINY_MACROS_H */
