#ifndef SHINY_DATA_H
#define SHINY_DATA_H

/* ShinyCommons.h must be included into all sources and headers as a first include */
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Part of `ShinyNode`. Contains latest timing data (after last "update").
 *
 * Particularly contains ticks spent inside node except enclosed nodes (aka `selfTicks`).
 * Also contains number of entries into the node (`entryCount`).
 */
typedef struct {
    uint32_t entryCount; /**< Number of entries to the node since last update. */
    shinytick_t selfTicks; /**< Ticks spent in this node since last update excluding time spent in enclosed nodes. */
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
    ShinyCountData entryCount; /**< Number of entries into this zone. */
    ShinyTickData selfTicks;   /**< Time (ticks) spent in zone excluding enclosed zones. */
    ShinyTickData childTicks;  /**< Time (ticks) spent in enclosed zones. */
} ShinyData;


SHINY_INLINE shinytick_t shinyData_totalTicksCur(const ShinyData * self) {
    return self->selfTicks.cur + self->childTicks.cur;
}

SHINY_INLINE float shinyData_totalTicksAvg(const ShinyData * self) {
    return self->selfTicks.avg + self->childTicks.avg;
}

/** Calculate new average values based on old average values, current values and damping.
 *
 * See also `shinyData_copyAverage()`, which does not take old average and
 * dumping into account and just overwrites averages with current values.
 */
SHINY_INLINE void shinyData_computeAverage(ShinyData * self, float damping) {
    self->entryCount.avg = self->entryCount.cur + damping * (self->entryCount.avg - self->entryCount.cur);
    self->selfTicks.avg = self->selfTicks.cur + damping * (self->selfTicks.avg - self->selfTicks.cur);
    self->childTicks.avg = self->childTicks.cur + damping * (self->childTicks.avg - self->childTicks.cur);
}

/** Copy values of `entryCount`, `selfTicks` and `childTicks` from `cur` to `avg`.
 *
 * In contrast with `shinyData_computeAverage` it just overwrites average
 * values with current ones (as if damping were 0). Used in "update clean"
 * versions of methods, which used either when damping is 0 or when it is
 * first update, i.e. there are no previous average values.
 */
SHINY_INLINE void shinyData_copyAverage(ShinyData * self) {
    self->entryCount.avg = (float) self->entryCount.cur;
    self->selfTicks.avg = (float) self->selfTicks.cur;
    self->childTicks.avg = (float) self->childTicks.cur;
}

SHINY_INLINE void shinyData_clearAll(ShinyData * self) {
    self->entryCount.cur = 0;
    self->entryCount.avg = 0;
    self->selfTicks.cur = 0;
    self->selfTicks.avg = 0;
    self->childTicks.cur = 0;
    self->childTicks.avg = 0;
}

SHINY_INLINE void shinyData_clearCurrent(ShinyData * self) {
    self->entryCount.cur = 0;
    self->selfTicks.cur = 0;
    self->childTicks.cur = 0;
}


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_DATA_H */
