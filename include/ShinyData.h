#ifndef SHINY_DATA_H
#define SHINY_DATA_H

#include "config.h"

/*---------------------------------------------------------------------------*/

typedef struct {
    uint32_t entryCount;
    shinytick_t selfTicks;
} ShinyLastData;


/*---------------------------------------------------------------------------*/

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
    self->entryCount.avg = self->entryCount.cur +
        a_damping * (self->entryCount.avg - self->entryCount.cur);
    self->selfTicks.avg = self->selfTicks.cur +
        a_damping * (self->selfTicks.avg - self->selfTicks.cur);
    self->childTicks.avg = self->childTicks.cur +
        a_damping * (self->childTicks.avg - self->childTicks.cur);
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

#endif /* SHINY_DATA_H */
