#ifndef SHINY_TOOLS_H
#define SHINY_TOOLS_H

/* ShinyCommons.h must be included into all sources and headers as a first include */
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float tickFreq;
    float invTickFreq;
    const char *suffix;
} ShinyTimeUnit;


SHINY_API const ShinyTimeUnit * shinyGetTimeUnit(float ticks);

SHINY_API void shinyGetTicks(shinytick_t * ticks);

SHINY_API shinytick_t shinyGetTickFreq(void);

SHINY_API float shinyGetTickInvFreq(void);

#ifdef __cplusplus
} // extern "C"
#endif


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_TOOLS_H */
