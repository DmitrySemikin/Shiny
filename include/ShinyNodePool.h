#ifndef SHINY_NODE_POOL_H
#define SHINY_NODE_POOL_H

/* ShinyCommons.h must be included into all sources and headers as a first include */
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#include "ShinyNode.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Pool of preallocated nodes.
 *
 * When new `ShinyNode` is needed, one should not allocate it,
 * but instead call `ShinyNodePool_newItem(pool)` on previosly
 * created node pool. This allows to significantly reduce amount
 * of allocations.
 *
 */
typedef struct _ShinyNodePool {

    /** All pools are organized into single linked list. This allows us to travers them all. */
    struct _ShinyNodePool * nextPool;


    /** Pointer to the next unused node in the pool. See also `ShinyNodePool_newItem()`. */
    ShinyNode * _nextItem;

    /**
     * Pointer to the "one after the last" node in the pool.
     * If "new node" returns this pointer, new pool should be allocatd
     */
    ShinyNode * endOfItems;

    /**
     * Pool of nodes.
     *
     * It is declared as 1-element array, but it is actually allocated so, that
     * it contains number of elements equal to the capacity of the pool.
     *
     * See also `ShinyNodePool_firstItem()`.
     */
    ShinyNode _items[1];

} ShinyNodePool;


/** Get pointer to the first `ShinyNode` in the pool (beginning of the pool). */
SHINY_INLINE ShinyNode * ShinyNodePool_firstItem(ShinyNodePool * self) {
    return &(self->_items[0]);
}

/** Get pointer, to the memory (nullified), which can be used as `ShinyNode` object. */
SHINY_INLINE ShinyNode * ShinyNodePool_newItem(ShinyNodePool * self) {
    return self->_nextItem++;
}

/** Create pool (allocate memory), which has number of `ShinyNode` objects equal to given `capacity`. */
ShinyNodePool * ShinyNodePool_create(uint32_t capacity);

/** Destroy (deallocate memory) this pool and all pools linked to it via `nextPool` member. */
void ShinyNodePool_destroy(ShinyNodePool * self);

/** Get memory occupied by all linked pools in bytes. */
uint32_t ShinyNodePool_memoryUsageChain(ShinyNodePool * firstPool);


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_NODE_POOL_H */
