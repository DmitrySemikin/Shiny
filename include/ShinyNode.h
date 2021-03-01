#ifndef SHINY_NODE_H
#define SHINY_NODE_H

// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"


#include "ShinyData.h"
#include "ShinyTools.h"

#if SHINY_IS_COMPILED == TRUE


/*---------------------------------------------------------------------------*/

typedef struct _ShinyNode {

    ShinyLastData _last;

    struct _ShinyZone* zone;
    struct _ShinyNode* parent;
    struct _ShinyNode* nextSibling;

    struct _ShinyNode* firstChild;
    struct _ShinyNode* lastChild;

    uint32_t childCount;
    uint32_t entryLevel;

    ShinyNodeCache* _cache;

    ShinyData data;

} ShinyNode;


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

SHINY_INLINE void ShinyNode_init(ShinyNode* self, ShinyNode* a_parent, struct _ShinyZone* a_zone, ShinyNodeCache* a_cache) {
    /* NOTE: all member variables are assumed to be zero when allocated */

    self->zone = a_zone;
    self->parent = a_parent;

    self->entryLevel = a_parent->entryLevel + 1;
    ShinyNode_addChild(a_parent, self);

    self->_cache = a_cache;
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

#if __cplusplus
} /* end of extern "C" */

template <class T>
void ShinyNode_enumerateNodes(const ShinyNode* a_node, T* a_this, void (T::*a_func)(const ShinyNode*)) {
    (a_this->*a_func)(a_node);

    if (a_node->firstChild) ShinyNode_enumerateNodes(a_node->firstChild, a_this, a_func);
    if (a_node->nextSibling) ShinyNode_enumerateNodes(a_node->nextSibling, a_this, a_func);
}

// TODO: Fix this
extern "C" { /* end of c++ */
#endif

#endif /* if SHINY_IS_COMPILED == TRUE */

#endif /* SHINY_NODE_H */
