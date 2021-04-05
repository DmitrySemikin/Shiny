#ifndef SHINY_ZONE_H
#define SHINY_ZONE_H

/* ShinyCommons.h must be included into all sources and headers as a first include */
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED

#ifdef __cplusplus
    #include <cstddef>
#else
    #include <stddef.h>
#endif

#include "ShinyData.h"
#include "ShinyStructForwardDeclarations.h"


#ifdef __cplusplus
extern "C" {
#endif


#define SHINY_ZONE_STATE_HIDDEN         0
#define SHINY_ZONE_STATE_INITIALIZED    1
#define SHINY_ZONE_STATE_UPDATING       2

struct ShinyZone {
    ShinyZone *  next;
    int          zoneState;
    const char * name;
    ShinyData    data;
};


SHINY_INLINE void shinyZone_init(ShinyZone * self, ShinyZone * previousZone) {
    self->zoneState = SHINY_ZONE_STATE_INITIALIZED;
    previousZone->next = self;
}

/* TODO: Seems to be unused. If true, remove it. */
SHINY_INLINE void shinyZone_uninit(ShinyZone * self) {
    self->zoneState = SHINY_ZONE_STATE_HIDDEN;
    self->next = NULL;
}

/** Clear current values of `ShinyData` of all Zones in the chain starting from this one. */
SHINY_API void shinyZone_preUpdateChain(ShinyZone * startZone);
SHINY_API void shinyZone_updateChain(ShinyZone * startZone, float damping);
SHINY_API void shinyZone_updateChainClean(ShinyZone * startZone);

SHINY_API void shinyZone_resetChain(ShinyZone * startZone);

SHINY_API ShinyZone* shinyZone_sortChain(ShinyZone ** startZone);

SHINY_INLINE float shinyZone_compare(ShinyZone * left, ShinyZone * right) {
    return left->data.selfTicks.avg - right->data.selfTicks.avg;
}

SHINY_API void shinyZone_clear(ShinyZone * self);

SHINY_API void shinyZone_enumerateZones(const ShinyZone * startZone, void (*functionToApply)(const ShinyZone*));

#ifdef __cplusplus
} // extern "C"
#endif



/* C++ API */

#ifdef __cplusplus

template <class T>
void shinyZone_enumerateZones(
        const ShinyZone * startZone,
        T * targetObject,
        void (T::*methodToApply)(const ShinyZone*)
) {
    (targetObject->*methodToApply)(startZone);

    if (startZone->next) {
        shinyZone_enumerateZones(startZone->next, targetObject, methodToApply);
    }
}

#endif // __cplusplus


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_ZONE_H */
