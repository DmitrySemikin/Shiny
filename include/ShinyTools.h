#ifndef SHINY_TOOLS_H
#define SHINY_TOOLS_H

// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"


/*---------------------------------------------------------------------------*/

typedef struct {
    float tickFreq;
    float invTickFreq;
    const char* suffix;
} ShinyTimeUnit;


/*---------------------------------------------------------------------------*/

SHINY_API const ShinyTimeUnit* ShinyGetTimeUnit(float ticks);

SHINY_API void ShinyGetTicks(shinytick_t *p);

SHINY_API shinytick_t ShinyGetTickFreq(void);

SHINY_API float ShinyGetTickInvFreq(void);

#endif /* SHINY_TOOLS_H */
