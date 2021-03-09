#ifndef SHINY_DATA_H
#define SHINY_DATA_H

/* ShinyCommons.h must be included into all sources and headers as a first include */
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t entryCount;
    shinytick_t selfTicks;
} ShinyLastData;


typedef struct {
    shinytick_t cur;
    float avg;
} ShinyTickData;


typedef struct {
    uint32_t cur;
    float avg;
} ShinyCountData;


typedef struct {
    ShinyCountData entryCount;
    ShinyTickData selfTicks;
    ShinyTickData childTicks;
} ShinyData;


SHINY_INLINE shinytick_t ShinyData_totalTicksCur(const ShinyData *self) {
    return self->selfTicks.cur + self->childTicks.cur;
}

SHINY_INLINE float ShinyData_totalTicksAvg(const ShinyData *self) {
    return self->selfTicks.avg + self->childTicks.avg;
}

SHINY_INLINE void ShinyData_computeAverage(ShinyData *self, float a_damping) {
    self->entryCount.avg = self->entryCount.cur + a_damping * (self->entryCount.avg - self->entryCount.cur);
    self->selfTicks.avg = self->selfTicks.cur + a_damping * (self->selfTicks.avg - self->selfTicks.cur);
    self->childTicks.avg = self->childTicks.cur + a_damping * (self->childTicks.avg - self->childTicks.cur);
}

SHINY_INLINE void ShinyData_copyAverage(ShinyData *self) {
    self->entryCount.avg = (float) self->entryCount.cur;
    self->selfTicks.avg = (float) self->selfTicks.cur;
    self->childTicks.avg = (float) self->childTicks.cur;
}

SHINY_INLINE void ShinyData_clearAll(ShinyData *self) {
    self->entryCount.cur = 0;
    self->entryCount.avg = 0;
    self->selfTicks.cur = 0;
    self->selfTicks.avg = 0;
    self->childTicks.cur = 0;
    self->childTicks.avg = 0;
}

SHINY_INLINE void ShinyData_clearCurrent(ShinyData *self) {
    self->entryCount.cur = 0;
    self->selfTicks.cur = 0;
    self->childTicks.cur = 0;
}


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_DATA_H */
