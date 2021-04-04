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

ShinyNode _shinyNode_dummy = {
    /* _last = */ { 0, 0 },
    /* zone = */ NULL,
    /* parent = */ NULL,
    /* nextSibling = */ NULL,
    /* firstChild = */ NULL,
    /* lastChild = */ NULL
};


/*---------------------------------------------------------------------------*/

void shinyNode_updateTree(ShinyNode* self, float a_damping) {
    ShinyNodeState *top = NULL;
    ShinyNode *node = self;

    for (;;) {
        do {
            top = shinyNodeState_push(top, node);
            node = node->firstChild;
        } while (node);

        for (;;) {
            node = shinyNodeState_finishAndGetNext(top, a_damping);
            top = shinyNodeState_pop(top);

            if (node) break;
            else if (!top) return;
        }
    }
}


/*---------------------------------------------------------------------------*/

void shinyNode_updateTreeClean(ShinyNode * self) {
    ShinyNodeState * top = NULL;
    ShinyNode * node = self;

    for (;;) {
        do {
            top = shinyNodeState_push(top, node);
            node = node->firstChild;
        } while (node);

        for (;;) {
            node = shinyNodeState_finishAndGetNextClean(top);
            top = shinyNodeState_pop(top);

            if (node) break;
            else if (!top) return;
        }
    }
}


/*---------------------------------------------------------------------------*/

const ShinyNode* shinyNode_findNextInTree(const ShinyNode* self) {
    if (self->firstChild) {
        return self->firstChild;

    } else if (self->nextSibling) {
        return self->nextSibling;

    } else {
        ShinyNode* pParent = self->parent;

        while (!shinyNode_isRoot(pParent)) {
            if (pParent->nextSibling) return pParent->nextSibling;
            else pParent = pParent->parent;
        }

        return NULL;
    }
}


/*---------------------------------------------------------------------------*/

void shinyNode_clear(ShinyNode* self) {
    memset(self, 0, sizeof(ShinyNode));
}


/*---------------------------------------------------------------------------*/

void shinyNode_enumerateNodes(const ShinyNode* a_node, void (*a_func)(const ShinyNode*)) {
    a_func(a_node);

    if (a_node->firstChild) shinyNode_enumerateNodes(a_node->firstChild, a_func);
    if (a_node->nextSibling) shinyNode_enumerateNodes(a_node->nextSibling, a_func);
}

#endif /* SHINY_IS_COMPILED */
