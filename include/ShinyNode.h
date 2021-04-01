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

/* TODO: Rename methods to start with small character. */
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

extern ShinyNode _ShinyNode_dummy;


/*---------------------------------------------------------------------------*/

SHINY_INLINE void ShinyNode_addChild(ShinyNode * self,  ShinyNode * childNode) {
    if (self->childCount++) {
        self->lastChild->nextSibling = childNode;
        self->lastChild = childNode;

    } else {
        self->lastChild = childNode;
        self->firstChild = childNode;
    }
}

SHINY_INLINE void ShinyNode_init(
    ShinyNode * self, 
    ShinyNode * parentNode, 
    ShinyZone * zone, 
    ShinyNodeCache * nodeCache
) {
    /* NOTE: all member variables are assumed to be zero when allocated */

    self->zone = zone;
    self->parent = parentNode;

    self->entryLevel = parentNode->entryLevel + 1;
    ShinyNode_addChild(parentNode, self);

    self->_cache = nodeCache;
}

SHINY_API void ShinyNode_updateTree(ShinyNode* self, float a_damping);
SHINY_API void ShinyNode_updateTreeClean(ShinyNode* self);

SHINY_INLINE void ShinyNode_destroy(ShinyNode* self) {
    *(self->_cache) = &_ShinyNode_dummy;
}

SHINY_INLINE void ShinyNode_appendTicks(ShinyNode * self, shinytick_t elapsedTicks) {
    self->_last.selfTicks += elapsedTicks;
}

SHINY_INLINE void ShinyNode_beginEntry(ShinyNode * self) {
    self->_last.entryCount++;
}

SHINY_INLINE int ShinyNode_isRoot(ShinyNode* self) {
    return (self->entryLevel == 0);
}

SHINY_INLINE int ShinyNode_isDummy(ShinyNode* self) {
    return (self == &_ShinyNode_dummy);
}

/** It is not really "isEqual", but rather "is child with this zone". */
SHINY_INLINE int ShinyNode_isEqual(ShinyNode* self, const ShinyNode* a_parent, const struct ShinyZone* a_zone) {
    return (self->parent == a_parent && self->zone == a_zone);
}

SHINY_API const ShinyNode* ShinyNode_findNextInTree(const ShinyNode* self);

SHINY_API void ShinyNode_clear(ShinyNode* self);

SHINY_API void ShinyNode_enumerateNodes(const ShinyNode* a_node, void (*a_func)(const ShinyNode*));

#ifdef __cplusplus
} /* end of extern "C" */
#endif



/* C++ API */
#ifdef __cplusplus

template <class T>
void ShinyNode_enumerateNodes(const ShinyNode* a_node, T* a_this, void (T::*a_func)(const ShinyNode*)) {
    (a_this->*a_func)(a_node);

    if (a_node->firstChild) ShinyNode_enumerateNodes(a_node->firstChild, a_this, a_func);
    if (a_node->nextSibling) ShinyNode_enumerateNodes(a_node->nextSibling, a_this, a_func);
}

#endif /* __cplusplus */



#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_NODE_H */
