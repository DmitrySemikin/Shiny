#ifndef SHINY_NODE_STATE_H
#define SHINY_NODE_STATE_H

/* ShinyCommons.h must be included into all sources and headers as a first include */
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#include "ShinyNode.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct _ShinyNodeState {
    ShinyNode * node;
    int         zoneUpdating;

    struct _ShinyNodeState *_prev;
} ShinyNodeState;


ShinyNodeState *ShinyNodeState_push(ShinyNodeState *a_top, ShinyNode *a_node);

ShinyNodeState *ShinyNodeState_pop(ShinyNodeState *a_top);

ShinyNode *ShinyNodeState_finishAndGetNext(ShinyNodeState *self, float a_damping);

ShinyNode *ShinyNodeState_finishAndGetNextClean(ShinyNodeState *self);


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_NODE_STATE_H */
