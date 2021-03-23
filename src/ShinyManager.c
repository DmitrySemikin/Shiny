// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#include "ShinyManager.h"

#ifdef __cplusplus
    #include <cstring>
    #include <cstdio>
    #include <cstdlib>
#else
    #include <string.h>
    #include <stdio.h>
    #include <stdlib.h>
#endif


/*---------------------------------------------------------------------------*/

#define TABLE_SIZE_INIT		256

/*---------------------------------------------------------------------------*/

ShinyManager Shiny_instance = {
    /* _lastTick = */ 0,
    /* _curNode = */ &Shiny_instance.rootNode,
    /* _tableMask = */ 0,
    /* _nodeTable = */ _ShinyManager_dummyNodeTable,
#if SHINY_LOOKUP_RATE == TRUE
    /* _lookupCount = */ 0,
    /* _lookupSuccessCount = */ 0,
#endif
    /* _tableSize = */ 1,
    /* nodeCount = */ 1,
    /* zoneCount = */ 1,
    /* _lastZone = */ &Shiny_instance.rootZone,
    /* _lastNodePool = */ NULL,
    /* _firstNodePool = */ NULL,
    /* rootNode = */ {
        /* _last = */ { 0, 0 },
        /* zone = */ &Shiny_instance.rootZone,
        /* parent = */ &Shiny_instance.rootNode,
        /* nextSibling = */ NULL,
        /* firstChild = */ NULL,
        /* lastChild = */ NULL,
        /* childCount = */ 0,
        /* entryLevel = */ 0,
        /* _cache = */ NULL,
        /* data = */ { { 0, 0 }, { 0, 0 }, { 0, 0 } }
    },
    /* rootZone = */ {
        /* next = */ NULL,
        /* zoneState = */ SHINY_ZONE_STATE_HIDDEN,
        /* name = */ "<unprofiled>",
        /* data = */ { { 0, 0 }, { 0, 0 }, { 0, 0 } }
    },
    /* damping = */ 0.9f,
    /* _initialized = */ FALSE,
    /* _firstUpdate = */ TRUE
};

ShinyNode * _ShinyManager_dummyNodeTable[] = { NULL };


/*---------------------------------------------------------------------------*/

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4311)
#endif

/* primary hash function */
SHINY_INLINE uint32_t _ShinyHashValueForNodeIndex(void * nodeParent, void * nodeZone) {

    uint32_t a = (uint32_t) nodeParent + (uint32_t) nodeZone;

    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    return a;
}

/*
 * secondary hash used as index offset: force it to be odd
 * so it's relatively prime to the power-of-two table size
 */
SHINY_INLINE uint32_t _ShinyHashValueForNodeIndexOffset(uint32_t currentIndex) {
    return ((currentIndex << 8) + (currentIndex >> 4)) | 1;
}

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	pragma warning (pop)
#endif


/*---------------------------------------------------------------------------*/

void ShinyManager_preLoad(ShinyManager *self) {
    if (!self->_initialized) {
        _ShinyManager_init(self);

        _ShinyManager_createNodeTable(self, TABLE_SIZE_INIT);
        _ShinyManager_createNodePool(self, TABLE_SIZE_INIT / 2);
    }
}


/*---------------------------------------------------------------------------*/

void ShinyManager_update(ShinyManager *self) {

    _ShinyManager_appendTicksToCurNode(self);
    ShinyZone_preUpdateChain(&self->rootZone);

    if (self->_firstUpdate || self->damping == 0) {
        self->_firstUpdate = FALSE;
        ShinyNode_updateTreeClean(&self->rootNode);
        ShinyZone_updateChainClean(&self->rootZone);

    } else {
        ShinyNode_updateTree(&self->rootNode, self->damping);
        ShinyZone_updateChain(&self->rootZone, self->damping);
    }
}


/*---------------------------------------------------------------------------*/

void ShinyManager_updateClean(ShinyManager *self) {

    _ShinyManager_appendTicksToCurNode(self);
    ShinyZone_preUpdateChain(&self->rootZone);

    self->_firstUpdate = FALSE;
    ShinyNode_updateTreeClean(&self->rootNode);
    ShinyZone_updateChainClean(&self->rootZone);
}


/*---------------------------------------------------------------------------*/

void ShinyManager_clear(ShinyManager *self) {
    ShinyManager_destroy(self);
    ShinyManager_preLoad(self);
}


/*---------------------------------------------------------------------------*/

void ShinyManager_destroy(ShinyManager *self) {
    ShinyManager_destroyNodes(self);
    ShinyManager_resetZones(self);
    _ShinyManager_uninit(self);
}


/*---------------------------------------------------------------------------*/

ShinyNode * _ShinyManager_lookupNode(
    ShinyManager *   self,
    ShinyNodeCache * nodeCache,
    ShinyZone *      zone
) {
    uint32_t currentHash = _ShinyHashValueForNodeIndex(self->_curNode, zone);
    uint32_t currentIndex = currentHash & self->_tableMask;
    ShinyNode * foundNode = self->_nodeTable[currentIndex];

    _ShinyManager_incLookup(self);
    _ShinyManager_incLookupSuccess(self);

    if (foundNode) {
        uint32_t step;

        /* check that foundNode->_parent == self->_curNode && foundNode->zone == zone */
        if (ShinyNode_isEqual(foundNode, self->_curNode, zone)) {
            return foundNode; /* found it! */
        }

        /* hash collision: */

        /* compute a secondary hash function for stepping */
        step = _ShinyHashValueForNodeIndexOffset(currentHash);

        for (;;) {
            _ShinyManager_incLookup(self);

            currentIndex = (currentIndex + step) & self->_tableMask;
            foundNode = self->_nodeTable[currentIndex];

            if (!foundNode) {
                break; /* found empty slot */
            } else if (ShinyNode_isEqual(foundNode, self->_curNode, zone)) {
                return foundNode; /* found it! */
            }
        }

        /* loop is guaranteed to end because the hash table is never full */
    }

    /* at this point `foundNode` points to empty slot in the table. */
    if (zone->zoneState == SHINY_ZONE_STATE_HIDDEN) { /* zone is not initialized */
        ShinyZone_init(zone, self->_lastZone);

        self->_lastZone = zone;
        self->zoneCount++;

        if (self->_initialized == FALSE) { /* first time init */
            _ShinyManager_init(self);

            _ShinyManager_createNodeTable(self, TABLE_SIZE_INIT);
            _ShinyManager_createNodePool(self, TABLE_SIZE_INIT / 2);

            /* initialization has invalidated `currentIndex`
             * we must compute `currentIndex` again
             */
            return _ShinyManager_createNode(self, nodeCache, zone);

            /* Althouth nodeCount is not updated
             * it includes rootNode so it adds up.
             */
        }
    }

    /* Check if we need to grow the table
     * we keep it at most 1/2 full to be very fast
     */
    if (self->_tableSize < 2 * self->nodeCount) {

        _ShinyManager_resizeNodeTable(self, 2 * self->_tableSize);
        _ShinyManager_resizeNodePool(self, self->nodeCount - 1);

        /* resize has invalidated nIndex
         * we must compute nIndex again
         */
        return _ShinyManager_createNode(self, nodeCache, zone);
    }

    self->nodeCount++;

    {
        ShinyNode * pNewNode = ShinyNodePool_newItem(self->_lastNodePool);
        ShinyNode_init(pNewNode, self->_curNode, zone, nodeCache);

        self->_nodeTable[currentIndex] = pNewNode;
        return pNewNode;
    }
}


/*---------------------------------------------------------------------------*/

void _ShinyManager_insertNode(ShinyManager * self, ShinyNode * node) {

    uint32_t newNodeHash = _ShinyHashValueForNodeIndex(node->parent, node->zone);
    uint32_t newNodeIndex = newNodeHash & self->_tableMask;

    if (self->_nodeTable[newNodeIndex]) {
        uint32_t step = _ShinyHashValueForNodeIndexOffset(newNodeHash);

        while (self->_nodeTable[newNodeIndex]) {
            newNodeIndex = (newNodeIndex + step) & self->_tableMask;
        }
    }

    self->_nodeTable[newNodeIndex] = node;
}


/*---------------------------------------------------------------------------*/

ShinyNode * _ShinyManager_createNode(
    ShinyManager * self, 
    ShinyNodeCache * nodeCache, 
    ShinyZone * zone
) {
    ShinyNode * newNode = ShinyNodePool_newItem(self->_lastNodePool);
    ShinyNode_init(newNode, self->_curNode, zone, nodeCache);

    self->nodeCount++;
    _ShinyManager_insertNode(self, newNode);
    return newNode;
}


/*---------------------------------------------------------------------------*/

void _ShinyManager_createNodePool(ShinyManager * self, uint32_t nodePoolCapacity) {
    self->_firstNodePool = ShinyNodePool_create(nodePoolCapacity);
    self->_lastNodePool = self->_firstNodePool;
}


/*---------------------------------------------------------------------------*/

void _ShinyManager_resizeNodePool(ShinyManager *self, uint32_t additionalPoolCapacity) {
    ShinyNodePool * pPool = ShinyNodePool_create(additionalPoolCapacity);
    self->_lastNodePool->nextPool = pPool;
    self->_lastNodePool = pPool;
}


/*---------------------------------------------------------------------------*/

void _ShinyManager_createNodeTable(ShinyManager * self, uint32_t tableSize) {

    self->_tableSize = tableSize;
    self->_tableMask = tableSize - 1;

    self->_nodeTable = (ShinyNodeTable*) malloc(sizeof(ShinyNode*) * tableSize);

    memset(self->_nodeTable, 0, tableSize * sizeof(ShinyNode*));
}


/*---------------------------------------------------------------------------*/

void _ShinyManager_resizeNodeTable(ShinyManager * self, uint32_t newTableSize) {

    ShinyNodePool * currentNodePool;

    free(self->_nodeTable);
    _ShinyManager_createNodeTable(self, newTableSize);

    currentNodePool = self->_firstNodePool;
    while (currentNodePool) {

        ShinyNode * currentNode = ShinyNodePool_firstItem(currentNodePool);

        while (currentNode != currentNodePool->_nextItem) {
            _ShinyManager_insertNode(self, currentNode++);
        }

        currentNodePool = currentNodePool->nextPool;
    }
}


/*---------------------------------------------------------------------------*/

void ShinyManager_resetZones(ShinyManager *self) {
    ShinyZone_resetChain(&self->rootZone);
    self->_lastZone = &self->rootZone;
    self->zoneCount = 1;
}


/*---------------------------------------------------------------------------*/

void ShinyManager_destroyNodes(ShinyManager *self) {
    if (self->_firstNodePool) {
        ShinyNodePool_destroy(self->_firstNodePool);
        self->_firstNodePool = NULL;
    }

    if (self->_nodeTable != _ShinyManager_dummyNodeTable) {
        free(self->_nodeTable);

        self->_nodeTable = _ShinyManager_dummyNodeTable;
        self->_tableSize = 1;
        self->_tableMask = 0;
    }

    self->_curNode = &self->rootNode;
    self->nodeCount = 1;

    _ShinyManager_init(self);
}


/*---------------------------------------------------------------------------*/

const char* ShinyManager_getOutputErrorString(ShinyManager *self) {
    if (self->_firstUpdate) return "!!! Profile data must first be updated !!!";
    else if (!self->_initialized) return "!!! No profiles where executed !!!";
    else return NULL;
}


/*---------------------------------------------------------------------------*/

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4996)
#endif

int ShinyManager_output(ShinyManager *self, const char *a_filename) {
    if (!a_filename) {
        ShinyManager_outputToStream(self, stdout);

    } else {
        FILE *file = fopen(a_filename, "w");
        if (!file) return FALSE;
        ShinyManager_outputToStream(self, file);
        fclose(file);
    }

    return TRUE;
}

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	pragma warning (pop)
#endif


/*---------------------------------------------------------------------------*/

void ShinyManager_outputToStream(ShinyManager *self, FILE *a_stream) {
    const char *error = ShinyManager_getOutputErrorString(self);

    if (error) {
        fwrite(error, 1, strlen(error), a_stream);
        fwrite("\n\n", 1, 2, a_stream);
        return;
    }

#if SHINY_OUTPUT_MODE & SHINY_OUTPUT_MODE_FLAT
    ShinyManager_sortZones(self);

    {
        int size = ShinyPrintZonesSize(self->zoneCount);
        char *buffer = (char*) malloc(size);
        ShinyPrintZones(buffer, &self->rootZone);
        fwrite(buffer, 1, size - 1, a_stream);
        fwrite("\n\n", 1, 2, a_stream);
        free(buffer);
    }
#endif

#if SHINY_OUTPUT_MODE & SHINY_OUTPUT_MODE_TREE
    {
        int size = ShinyPrintNodesSize(self->nodeCount);
        char *buffer = (char*) malloc(size);
        ShinyPrintNodes(buffer, &self->rootNode);
        fwrite(buffer, 1, size - 1, a_stream);
        fwrite("\n\n", 1, 2, a_stream);
        free(buffer);
    }
#endif
}

#endif /* SHINY_IS_COMPILED */
