// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"

#include "ShinyNodePool.h"
#include "ShinyTools.h"

#include <stdlib.h>
#include <memory.h>

#if SHINY_IS_COMPILED == TRUE


/*---------------------------------------------------------------------------*/

ShinyNodePool* ShinyNodePool_create(uint32_t a_items) {
    ShinyNodePool* pPool = (ShinyNodePool*)
        malloc(sizeof(ShinyNodePool) + sizeof(ShinyNode) * (a_items - 1));

    pPool->nextPool = NULL;
    pPool->_nextItem = &pPool->_items[0];
    pPool->endOfItems = &pPool->_items[a_items];

    memset(&pPool->_items[0], 0, a_items * sizeof(ShinyNode));
    return pPool;
}


/*---------------------------------------------------------------------------*/

uint32_t ShinyNodePool_memoryUsageChain(ShinyNodePool *first) {
    uint32_t bytes = (uint32_t) ((char*) first->endOfItems - (char*) first);
    ShinyNodePool *pool = first->nextPool;

    while (pool) {
        bytes += (uint32_t) ((char*) pool->endOfItems - (char*) pool);
        pool = pool->nextPool;
    }

    return bytes;
}


/*---------------------------------------------------------------------------*/

void ShinyNodePool_destroy(ShinyNodePool *self) {
    ShinyNode* firstNode = ShinyNodePool_firstItem(self);
    ShinyNode* lastNode = self->_nextItem;

    while (firstNode != lastNode)
        ShinyNode_destroy(firstNode++);

    /* TODO: make this into a loop or a tail recursion */
    if (self->nextPool) ShinyNodePool_destroy(self->nextPool);
    free(self);
}

#endif
