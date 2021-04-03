#ifndef SHINY_NODE_STATE_H
#define SHINY_NODE_STATE_H

/* ShinyCommons.h must be included into all sources and headers as a first include */
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#include "ShinyNode.h"


#ifdef __cplusplus
extern "C" {
#endif


struct ShinyNodeState;
typedef  struct ShinyNodeState  ShinyNodeState;

struct ShinyNodeState {
    ShinyNode      * node;
    int              zoneUpdating;
    ShinyNodeState * _previousState;
};


/** Create new State on top of `topState`, initialize it from `node` and return. */
ShinyNodeState * ShinyNodeState_push(ShinyNodeState * topState, ShinyNode * node);

/** Destroy given `topState` and return its "previous". */
ShinyNodeState * ShinyNodeState_pop(ShinyNodeState * topState);

ShinyNode * ShinyNodeState_finishAndGetNext(ShinyNodeState * self, float damping);

ShinyNode * ShinyNodeState_finishAndGetNextClean(ShinyNodeState * self);


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_NODE_STATE_H */
