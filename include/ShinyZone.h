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


#ifdef __cplusplus
extern "C" {
#endif


#define SHINY_ZONE_STATE_HIDDEN         0
#define SHINY_ZONE_STATE_INITIALIZED    1
#define SHINY_ZONE_STATE_UPDATING       2

typedef struct _ShinyZone {
    struct _ShinyZone * next;
    int                 zoneState;
    const char *        name;
    ShinyData           data;
} ShinyZone;


SHINY_INLINE void ShinyZone_init(ShinyZone * self, ShinyZone * a_prev) {
    self->zoneState = SHINY_ZONE_STATE_INITIALIZED;
    a_prev->next = self;
}

/* TODO: Seems to be unused. If true, remove it. */
SHINY_INLINE void ShinyZone_uninit(ShinyZone * self) {
    self->zoneState = SHINY_ZONE_STATE_HIDDEN;
    self->next = NULL;
}

SHINY_API void ShinyZone_preUpdateChain(ShinyZone *first);
SHINY_API void ShinyZone_updateChain(ShinyZone *first, float a_damping);
SHINY_API void ShinyZone_updateChainClean(ShinyZone *first);

SHINY_API void ShinyZone_resetChain(ShinyZone *first);

SHINY_API ShinyZone* ShinyZone_sortChain(ShinyZone **first);

SHINY_INLINE float ShinyZone_compare(ShinyZone *a, ShinyZone *b) {
    return a->data.selfTicks.avg - b->data.selfTicks.avg;
}

SHINY_API void ShinyZone_clear(ShinyZone* self);

SHINY_API void ShinyZone_enumerateZones(const ShinyZone* a_zone, void (*a_func)(const ShinyZone*));

#ifdef __cplusplus
} // extern "C"
#endif



/* C++ API */

#ifdef __cplusplus
template <class T>
void ShinyZone_enumerateZones(const ShinyZone* a_zone, T* a_this, void (T::*a_func)(const ShinyZone*)) {
    (a_this->*a_func)(a_zone);

    if (a_zone->next) ShinyZone_enumerateZones(a_zone->next, a_this, a_func);
}

#endif // __cplusplus


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_ZONE_H */
