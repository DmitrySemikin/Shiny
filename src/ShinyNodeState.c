// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED

#include <stdlib.h>

#include "ShinyNodeState.h"
#include "ShinyNode.h"
#include "ShinyZone.h"

/*---------------------------------------------------------------------------*/

ShinyNodeState* ShinyNodeState_push(ShinyNodeState *a_top, ShinyNode *a_node) {
    ShinyZone *zone = a_node->zone;
    ShinyNodeState *self = (ShinyNodeState*) malloc(sizeof(ShinyNodeState));
    self->node = a_node;
    self->_prev = a_top;

    a_node->data.selfTicks.cur = a_node->_last.selfTicks;
    a_node->data.entryCount.cur = a_node->_last.entryCount;

    zone->data.selfTicks.cur += a_node->_last.selfTicks;
    zone->data.entryCount.cur += a_node->_last.entryCount;

    a_node->data.childTicks.cur = 0;
    a_node->_last.selfTicks = 0;
    a_node->_last.entryCount = 0;

    self->zoneUpdating = zone->zoneState != SHINY_ZONE_STATE_UPDATING;
    if (self->zoneUpdating) {
        zone->zoneState = SHINY_ZONE_STATE_UPDATING;
    } else {
        zone->data.childTicks.cur -= a_node->data.selfTicks.cur;
    }

    return self;
}

/*---------------------------------------------------------------------------*/

ShinyNodeState* ShinyNodeState_pop(ShinyNodeState *a_top) {
    ShinyNodeState *prev = a_top->_prev;
    free(a_top);
    return prev;
}

/*---------------------------------------------------------------------------*/

ShinyNode* ShinyNodeState_finishAndGetNext(ShinyNodeState *self, float a_damping) {
    ShinyNode *node = self->node;
    ShinyZone *zone = node->zone;

    if (self->zoneUpdating) {
        zone->data.childTicks.cur += node->data.childTicks.cur;
        zone->zoneState = SHINY_ZONE_STATE_INITIALIZED;
    }

    ShinyData_computeAverage(&node->data, a_damping);

    if (!ShinyNode_isRoot(node))
        node->parent->data.childTicks.cur += node->data.selfTicks.cur + node->data.childTicks.cur;

    return node->nextSibling;
}


/*---------------------------------------------------------------------------*/

ShinyNode* ShinyNodeState_finishAndGetNextClean(ShinyNodeState *self) {
    ShinyNode *node = self->node;
    ShinyZone *zone = node->zone;

    if (self->zoneUpdating) {
        zone->data.childTicks.cur += node->data.childTicks.cur;
        zone->zoneState = SHINY_ZONE_STATE_INITIALIZED;
    }

    ShinyData_copyAverage(&node->data);

    if (!ShinyNode_isRoot(node))
        node->parent->data.childTicks.cur += node->data.selfTicks.cur + node->data.childTicks.cur;

    return node->nextSibling;
}

#endif /* SHINY_IS_COMPILED */
