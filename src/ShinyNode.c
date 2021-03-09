// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED

#ifdef __cplusplus
    #include <cstring>
#else
    #include <string.h>
#endif

#include "ShinyNode.h"
#include "ShinyZone.h"
#include "ShinyNodeState.h"



/*---------------------------------------------------------------------------*/

ShinyNode _ShinyNode_dummy = {
    /* _last = */ { 0, 0 },
    /* zone = */ NULL,
    /* parent = */ NULL,
    /* nextSibling = */ NULL,
    /* firstChild = */ NULL,
    /* lastChild = */ NULL
};


/*---------------------------------------------------------------------------*/

void ShinyNode_updateTree(ShinyNode* first, float a_damping) {
    ShinyNodeState *top = NULL;
    ShinyNode *node = first;

    for (;;) {
        do {
            top = ShinyNodeState_push(top, node);
            node = node->firstChild;
        } while (node);

        for (;;) {
            node = ShinyNodeState_finishAndGetNext(top, a_damping);
            top = ShinyNodeState_pop(top);

            if (node) break;
            else if (!top) return;
        }
    }
}


/*---------------------------------------------------------------------------*/

void ShinyNode_updateTreeClean(ShinyNode* first) {
    ShinyNodeState *top = NULL;
    ShinyNode *node = first;

    for (;;) {
        do {
            top = ShinyNodeState_push(top, node);
            node = node->firstChild;
        } while (node);

        for (;;) {
            node = ShinyNodeState_finishAndGetNextClean(top);
            top = ShinyNodeState_pop(top);

            if (node) break;
            else if (!top) return;
        }
    }
}


/*---------------------------------------------------------------------------*/

const ShinyNode* ShinyNode_findNextInTree(const ShinyNode* self) {
    if (self->firstChild) {
        return self->firstChild;

    } else if (self->nextSibling) {
        return self->nextSibling;

    } else {
        ShinyNode* pParent = self->parent;

        while (!ShinyNode_isRoot(pParent)) {
            if (pParent->nextSibling) return pParent->nextSibling;
            else pParent = pParent->parent;
        }

        return NULL;
    }
}


/*---------------------------------------------------------------------------*/

void ShinyNode_clear(ShinyNode* self) {
    memset(self, 0, sizeof(ShinyNode));
}


/*---------------------------------------------------------------------------*/

void ShinyNode_enumerateNodes(const ShinyNode* a_node, void (*a_func)(const ShinyNode*)) {
    a_func(a_node);

    if (a_node->firstChild) ShinyNode_enumerateNodes(a_node->firstChild, a_func);
    if (a_node->nextSibling) ShinyNode_enumerateNodes(a_node->nextSibling, a_func);
}

#endif /* SHINY_IS_COMPILED */
