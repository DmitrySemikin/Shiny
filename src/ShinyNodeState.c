// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif


#include "ShinyNodeState.h"
#include "ShinyNode.h"
#include "ShinyZone.h"

/*---------------------------------------------------------------------------*/

ShinyNodeState * shinyNodeState_push(ShinyNodeState * topState, ShinyNode * node) {
    ShinyZone * zone = node->zone;
    ShinyNodeState * self = (ShinyNodeState*) malloc(sizeof(ShinyNodeState));
    self->node = node;
    self->_previousState = topState;

    node->data.selfTicks.cur = node->_last.selfTicks;
    node->data.entryCount.cur = node->_last.entryCount;

    zone->data.selfTicks.cur += node->_last.selfTicks;
    zone->data.entryCount.cur += node->_last.entryCount;

    node->data.childTicks.cur = 0;
    node->_last.selfTicks = 0;
    node->_last.entryCount = 0;

    self->zoneUpdating = zone->zoneState != SHINY_ZONE_STATE_UPDATING;
    if (self->zoneUpdating) {
        zone->zoneState = SHINY_ZONE_STATE_UPDATING;
    } else {
        zone->data.childTicks.cur -= node->data.selfTicks.cur;
    }

    return self;
}

/*---------------------------------------------------------------------------*/

ShinyNodeState * shinyNodeState_pop(ShinyNodeState * topState) {
    ShinyNodeState * previousState = topState->_previousState;
    free(topState);
    return previousState;
}

/*---------------------------------------------------------------------------*/

ShinyNode* shinyNodeState_finishAndGetNext(ShinyNodeState * self, float damping) {
    ShinyNode *node = self->node;
    ShinyZone *zone = node->zone;

    if (self->zoneUpdating) {
        zone->data.childTicks.cur += node->data.childTicks.cur;
        zone->zoneState = SHINY_ZONE_STATE_INITIALIZED;
    }

    shinyData_computeAverage(&node->data, damping);

    if (!shinyNode_isRoot(node))
        node->parent->data.childTicks.cur += node->data.selfTicks.cur + node->data.childTicks.cur;

    return node->nextSibling;
}


/*---------------------------------------------------------------------------*/

ShinyNode* shinyNodeState_finishAndGetNextClean(ShinyNodeState * self) {
    ShinyNode * node = self->node;
    ShinyZone * zone = node->zone;

    if (self->zoneUpdating) {
        zone->data.childTicks.cur += node->data.childTicks.cur;
        zone->zoneState = SHINY_ZONE_STATE_INITIALIZED;
    }

    shinyData_copyAverage(&node->data);

    if (!shinyNode_isRoot(node))
        node->parent->data.childTicks.cur += node->data.selfTicks.cur + node->data.childTicks.cur;

    return node->nextSibling;
}

#endif /* SHINY_IS_COMPILED */
