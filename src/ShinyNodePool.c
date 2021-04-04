// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#ifdef __cplusplus
    #include <cstdlib>
    #include <cstring>
#else
    #include <stdlib.h>
    #include <string.h>
#endif


#include "ShinyNodePool.h"


/*---------------------------------------------------------------------------*/

ShinyNodePool * shinyNodePool_create(uint32_t capacity) {

    ShinyNodePool * pool =
        (ShinyNodePool*)malloc(sizeof(ShinyNodePool) + sizeof(ShinyNode) * (capacity - 1));

    pool->nextPool = NULL;
    pool->_nextItem = &pool->_items[0];
    pool->endOfItems = &pool->_items[capacity];

    memset(&pool->_items[0], 0, capacity * sizeof(ShinyNode));

    return pool;
}


/*---------------------------------------------------------------------------*/

uint32_t shinyNodePool_memoryUsageChain(ShinyNodePool * firstPool) {

    /* TODO: We can put first iteration into the loop. */
    uint32_t bytes = (uint32_t) ((char*) firstPool->endOfItems - (char*) firstPool);

    ShinyNodePool * currentPool = firstPool->nextPool;
    while (currentPool) {
        bytes += (uint32_t) ((char*) currentPool->endOfItems - (char*) currentPool);
        currentPool = currentPool->nextPool;
    }

    return bytes;
}


/*---------------------------------------------------------------------------*/

void shinyNodePool_destroy(ShinyNodePool * self) {

    ShinyNode * currentNode = shinyNodePool_firstItem(self);
    ShinyNode * lastNode = self->_nextItem;

    while (currentNode != lastNode) {
        shinyNode_destroy(currentNode++);
    }

    /* TODO: make this into a loop or a tail recursion */
    if (self->nextPool) {
        shinyNodePool_destroy(self->nextPool);
    }

    free(self);
}


/*---------------------------------------------------------------------------*/

#endif /* SHINY_IS_COMPILED */
