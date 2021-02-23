#ifndef SHINY_NODE_STATE_H
#define SHINY_NODE_STATE_H

// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"


#include "ShinyNode.h"

#if SHINY_IS_COMPILED == TRUE


/*---------------------------------------------------------------------------*/

typedef struct _ShinyNodeState {
    ShinyNode *node;
    int zoneUpdating;

    struct _ShinyNodeState *_prev;
} ShinyNodeState;


/*---------------------------------------------------------------------------*/

ShinyNodeState* ShinyNodeState_push(ShinyNodeState *a_top, ShinyNode *a_node);
ShinyNodeState* ShinyNodeState_pop(ShinyNodeState *a_top);

ShinyNode* ShinyNodeState_finishAndGetNext(ShinyNodeState *self, float a_damping);
ShinyNode* ShinyNodeState_finishAndGetNextClean(ShinyNodeState *self);

#endif /* if SHINY_IS_COMPILED == TRUE */

#endif /* SHINY_NODE_STATE_H */
