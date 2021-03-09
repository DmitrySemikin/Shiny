#ifndef SHINY_NODE_POOL_H
#define SHINY_NODE_POOL_H

/* ShinyCommons.h must be included into all sources and headers as a first include */
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#include "ShinyNode.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct _ShinyNodePool {

    struct _ShinyNodePool *nextPool;

    ShinyNode *_nextItem;
    ShinyNode *endOfItems;

    ShinyNode _items[1];

} ShinyNodePool;



SHINY_INLINE ShinyNode *ShinyNodePool_firstItem(ShinyNodePool *self) {
    return &(self->_items[0]);
}

SHINY_INLINE ShinyNode *ShinyNodePool_newItem(ShinyNodePool *self) {
    return self->_nextItem++;
}

ShinyNodePool *ShinyNodePool_create(uint32_t a_items);
void ShinyNodePool_destroy(ShinyNodePool *self);

uint32_t ShinyNodePool_memoryUsageChain(ShinyNodePool *first);


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_NODE_POOL_H */
