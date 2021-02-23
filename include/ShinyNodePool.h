#ifndef SHINY_NODE_POOL_H
#define SHINY_NODE_POOL_H

// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"


#include "ShinyNode.h"

#if SHINY_IS_COMPILED == TRUE


/*---------------------------------------------------------------------------*/

typedef struct _ShinyNodePool {

    struct _ShinyNodePool* nextPool;

    ShinyNode *_nextItem;
    ShinyNode *endOfItems;

    ShinyNode _items[1];

} ShinyNodePool;


/*---------------------------------------------------------------------------*/

SHINY_INLINE ShinyNode* ShinyNodePool_firstItem(ShinyNodePool *self) {
    return &(self->_items[0]);
}

SHINY_INLINE ShinyNode* ShinyNodePool_newItem(ShinyNodePool *self) {
    return self->_nextItem++;
}

ShinyNodePool* ShinyNodePool_create(uint32_t a_items);
void ShinyNodePool_destroy(ShinyNodePool *self);

uint32_t ShinyNodePool_memoryUsageChain(ShinyNodePool *first);


#endif /* if SHINY_IS_COMPILED == TRUE */

#endif /* SHINY_NODE_POOL_H */
