#ifndef SHINY_NODE_H
#define SHINY_NODE_H

/* ShinyCommons.h must be included into all sources and headers as a first include */
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#include "ShinyData.h"
#include "ShinyStructForwardDeclarations.h"
#include "ShinyTools.h"


#ifdef __cplusplus
extern "C" {
#endif

/* TODO: Add documentation. */
struct ShinyNode {

    ShinyLastData _last;     /**< Timing data since last "update". */

    ShinyZone * zone;        /**< Zone corresponding to this node. */

    ShinyNode * parent;      /**< Parent node of this node. */
    ShinyNode * nextSibling; /**< Defines linked list of siblings in node tree. */
    ShinyNode * firstChild;  /**< First element in the linked list of siblings of next level. */
    ShinyNode * lastChild;   /**< Last element in the linked list of siblings of the next level. */

    uint32_t childCount;     /**< Number of children. */
    uint32_t entryLevel;     /**< Level of the node in the node tree (i.e. root hast 0, it's children 1 etc.). */

    ShinyNodeCache * _cache; /**< Back reference - used to clean it up, when the node is destroyed. */

    ShinyData data;          /**< This is probably aggregated data (here it is stored during "update". */
};


/*---------------------------------------------------------------------------*/

extern ShinyNode _shinyNode_dummy;


/*---------------------------------------------------------------------------*/

SHINY_INLINE void shinyNode_addChild(ShinyNode * self, ShinyNode * childNode) {
    if (self->childCount++) {
        self->lastChild->nextSibling = childNode;
        self->lastChild = childNode;

    } else {
        self->lastChild = childNode;
        self->firstChild = childNode;
    }
}

SHINY_INLINE void shinyNode_init(
    ShinyNode * self, 
    ShinyNode * parentNode, 
    ShinyZone * zone, 
    ShinyNodeCache * nodeCache
) {
    /* NOTE: all member variables are assumed to be zero when allocated */

    self->zone = zone;
    self->parent = parentNode;

    self->entryLevel = parentNode->entryLevel + 1;
    shinyNode_addChild(parentNode, self);

    self->_cache = nodeCache;
}

SHINY_API void shinyNode_updateTree(ShinyNode * self, float damping);
SHINY_API void shinyNode_updateTreeClean(ShinyNode * self);

SHINY_INLINE void shinyNode_destroy(ShinyNode * self) {
    *(self->_cache) = &_shinyNode_dummy;
}

SHINY_INLINE void shinyNode_appendTicks(ShinyNode * self, shinytick_t elapsedTicks) {
    self->_last.selfTicks += elapsedTicks;
}

SHINY_INLINE void shinyNode_beginEntry(ShinyNode * self) {
    self->_last.entryCount++;
}

SHINY_INLINE int shinyNode_isRoot(ShinyNode * self) {
    return (self->entryLevel == 0);
}

SHINY_INLINE int shinyNode_isDummy(ShinyNode * self) {
    return (self == &_shinyNode_dummy);
}

/** It is not really "isEqual", but rather "is child with this zone".
 *
 *  Node's place in the node-tree is defined by it's parent (context)
 *  and corresponding Zone, which it represents. So this function checks,
 *  that this data stored in the given Node (`self`) matches other two
 *  input parameters.
 */
SHINY_INLINE int shinyNode_isEqual(
        ShinyNode * self,
        const ShinyNode * parentNode,
        const struct ShinyZone* zone
) {
    return (self->parent == parentNode && self->zone == zone);
}

SHINY_API const ShinyNode* shinyNode_findNextInTree(const ShinyNode * self);

SHINY_API void shinyNode_clear(ShinyNode * self);

/** Apply given function to `startNode` and to all further siblings and children of it. */
SHINY_API void shinyNode_enumerateNodes(
        const ShinyNode * startNode,
        void (*functionToApply)(const ShinyNode*)
);

#ifdef __cplusplus
} /* end of extern "C" */
#endif



/* C++ API */
#ifdef __cplusplus

/** Apply given `methodToApply` to `targetObject` for given `startNode` and all its further siblings and children
 *
 * Note, that `targetObject` does not change, when we navigate from one node to another.
 */
template <class T>
void shinyNode_enumerateNodes(
        const ShinyNode * startNode,
        T * targetObject,
        void (T::*methodToApply)(const ShinyNode*)
) {
    (targetObject->*methodToApply)(startNode);

    if (startNode->firstChild) {
        shinyNode_enumerateNodes(startNode->firstChild, targetObject, methodToApply);
    }
    if (startNode->nextSibling) {
        shinyNode_enumerateNodes(startNode->nextSibling, targetObject, methodToApply);
    }
}

#endif /* __cplusplus */



#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_NODE_H */
