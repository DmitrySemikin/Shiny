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

struct _ShinyNode {

    ShinyLastData _last;

    ShinyZone * zone;
    ShinyNode * parent;
    ShinyNode * nextSibling;

    ShinyNode * firstChild;
    ShinyNode * lastChild;

    uint32_t childCount;
    uint32_t entryLevel;

    ShinyNodeCache * _cache;

    ShinyData data;

};


/*---------------------------------------------------------------------------*/

extern ShinyNode _ShinyNode_dummy;


/*---------------------------------------------------------------------------*/

SHINY_INLINE void ShinyNode_addChild(ShinyNode* self,  ShinyNode* a_child) {
    if (self->childCount++) {
        self->lastChild->nextSibling = a_child;
        self->lastChild = a_child;

    } else {
        self->lastChild = a_child;
        self->firstChild = a_child;
    }
}

SHINY_INLINE void ShinyNode_init(
    ShinyNode * self, 
    ShinyNode * parentNode, 
    struct _ShinyZone * zone, 
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

SHINY_INLINE void ShinyNode_appendTicks(ShinyNode* self, shinytick_t a_elapsedTicks) {
    self->_last.selfTicks += a_elapsedTicks;
}

SHINY_INLINE void ShinyNode_beginEntry(ShinyNode* self) {
    self->_last.entryCount++;
}

SHINY_INLINE int ShinyNode_isRoot(ShinyNode* self) {
    return (self->entryLevel == 0);
}

SHINY_INLINE int ShinyNode_isDummy(ShinyNode* self) {
    return (self == &_ShinyNode_dummy);
}

SHINY_INLINE int ShinyNode_isEqual(ShinyNode* self, const ShinyNode* a_parent, const struct _ShinyZone* a_zone) {
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
