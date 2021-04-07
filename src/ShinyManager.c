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

ShinyManager shinyManagerInstance = {
    /* _lastTick = */ 0,
    /* _currentNode = */ &shinyManagerInstance._rootNode,
    /* _rootNode = */ {
        /* _last = */ { 0, 0 },
        /* zone = */ &shinyManagerInstance._firstZone,
        /* parent = */ &shinyManagerInstance._rootNode,
        /* nextSibling = */ NULL,
        /* firstChild = */ NULL,
        /* lastChild = */ NULL,
        /* childCount = */ 0,
        /* entryLevel = */ 0,
        /* _cache = */ NULL,
        /* data = */ { { 0, 0 }, { 0, 0 }, { 0, 0 } }
    },
    /* _firstZone = */ {
        /* next = */ NULL,
        /* zoneState = */ SHINY_ZONE_STATE_HIDDEN,
        /* name = */ "<unprofiled>",
        /* data = */ { { 0, 0 }, { 0, 0 }, { 0, 0 } }
    },
    /* _lastZone = */ &shinyManagerInstance._firstZone,
    /* _nodeCount = */ 1,
    /* _zoneCount = */ 1,
    /* _nodeTable = */ _shinyManager_dummyNodeTable,
    /* _tableSize = */ 1,
    /* _tableMask = */ 0,
    /* _firstNodePool = */ NULL,
    /* _lastNodePool = */ NULL,
    /* damping = */ 0.9f,
    /* _initialized = */ FALSE,
    /* _firstUpdate = */ TRUE

#if SHINY_LOOKUP_RATE == TRUE
    ,
    /* _lookupCount = */ 0,
    /* _lookupSuccessCount = */ 0
#endif

};

ShinyNode * _shinyManager_dummyNodeTable[] = { NULL };


/*---------------------------------------------------------------------------*/

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
    #pragma warning (push)
    #pragma warning (disable: 4311)
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

/* primary hash function */
SHINY_INLINE uint32_t _shinyHashValueForNodeIndex(void * nodeParent, void * nodeZone) {

    uint32_t a = (uint32_t) nodeParent + (uint32_t) nodeZone;

    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    return a;
}

/*
 * secondary hash used as index offset: force it to be odd
 * so it's relatively prime to the power-of-two table size
 */
SHINY_INLINE uint32_t _shinyHashValueForNodeIndexOffset(uint32_t currentIndex) {
    return ((currentIndex << 8) + (currentIndex >> 4)) | 1;
}

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
    #pragma warning (pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif


/*---------------------------------------------------------------------------*/

void shinyManager_preLoad(ShinyManager * self) {
    if (!self->_initialized) {
        _shinyManager_init(self);

        _shinyManager_createNodeTable(self, TABLE_SIZE_INIT);
        _shinyManager_createFirstNodePool(self, TABLE_SIZE_INIT / 2);
    }
}


/*---------------------------------------------------------------------------*/

void shinyManager_update(ShinyManager * self) {

    _shinyManager_appendTicksToCurNode(self);
    shinyZone_preUpdateChain(&self->_firstZone);

    if (self->_firstUpdate || self->damping == 0) {

        self->_firstUpdate = FALSE;
        shinyNode_updateTreeClean(&self->_rootNode);
        shinyZone_updateChainClean(&self->_firstZone);

    } else {

        shinyNode_updateTree(&self->_rootNode, self->damping);
        shinyZone_updateChain(&self->_firstZone, self->damping);

    }

}


/*---------------------------------------------------------------------------*/

void shinyManager_updateClean(ShinyManager * self) {

    _shinyManager_appendTicksToCurNode(self);
    shinyZone_preUpdateChain(&self->_firstZone);

    self->_firstUpdate = FALSE;
    shinyNode_updateTreeClean(&self->_rootNode);
    shinyZone_updateChainClean(&self->_firstZone);
}


/*---------------------------------------------------------------------------*/

void shinyManager_clear(ShinyManager * self) {
    shinyManager_destroy(self);
    shinyManager_preLoad(self);
}


/*---------------------------------------------------------------------------*/

void shinyManager_destroy(ShinyManager * self) {
    shinyManager_destroyNodes(self);
    shinyManager_resetZones(self);
    _shinyManager_uninit(self);
}


/*---------------------------------------------------------------------------*/

/*
 * 1. index <- self->currentNode, zone
 * 2. If found (curNode is parent of Found node), then return foundNode (is stored in cache)
 * 3. Else create new node, where self->currentNode is parent and put it into index
 */
ShinyNode * _shinyManager_lookupNode(
    ShinyManager *   self,
    ShinyNodeCache * a_cache,
    ShinyZone *      a_zone
) {
    uint32_t currentHash = _shinyHashValueForNodeIndex(self->_currentNode, a_zone);
    uint32_t currentIndex = currentHash & self->_tableMask;
    ShinyNode * foundNode = self->_nodeTable[currentIndex];

    _shinyManager_incLookup(self);
    _shinyManager_incLookupSuccess(self);

    if (foundNode) {
        uint32_t step;

        /* check that foundNode->_parent == self->_currentNode && foundNode->zone == zone */
        if (shinyNode_isEqual(foundNode, self->_currentNode, a_zone)) {
            return foundNode; /* found it! */
        }

        /* hash collision: */

        /* compute a secondary hash function for stepping */
        step = _shinyHashValueForNodeIndexOffset(currentHash);

        for (;;) {
            _shinyManager_incLookup(self);

            currentIndex = (currentIndex + step) & self->_tableMask;
            foundNode = self->_nodeTable[currentIndex];

            if (!foundNode) {
                break; /* found empty slot */
            } else if (shinyNode_isEqual(foundNode, self->_currentNode, a_zone)) {
                return foundNode; /* found it! */
            }
        }

        /* loop is guaranteed to end because the hash table is never full */
    }

    /* at this point `foundNode` points to empty slot in the table. */
    if (a_zone->zoneState == SHINY_ZONE_STATE_HIDDEN) { /* zone is not initialized */
        shinyZone_init(a_zone, self->_lastZone);

        self->_lastZone = a_zone;
        self->_zoneCount++;

        if (self->_initialized == FALSE) { /* first time init */
            _shinyManager_init(self);

            _shinyManager_createNodeTable(self, TABLE_SIZE_INIT);
            _shinyManager_createFirstNodePool(self, TABLE_SIZE_INIT / 2);

            /* initialization has invalidated `currentIndex`
             * we must compute `currentIndex` again
             */
            return _shinyManager_createNode(self, a_cache, a_zone);

            /* Althouth _nodeCount is not updated
             * it includes _rootNode so it adds up.
             */
        }
    }

    /* Check if we need to grow the table
     * we keep it at most 1/2 full to be very fast
     */
    if (self->_tableSize < 2 * self->_nodeCount) {

        _shinyManager_resizeNodeTable(self, 2 * self->_tableSize);
        _shinyManager_createAdditionalNodePool(self, self->_nodeCount - 1);

        /* resize has invalidated nIndex
         * we must compute nIndex again
         */
        return _shinyManager_createNode(self, a_cache, a_zone);
    }

    self->_nodeCount++;

    {
        ShinyNode * newNode = shinyNodePool_newItem(self->_lastNodePool);
        shinyNode_init(newNode, self->_currentNode, a_zone, a_cache);

        self->_nodeTable[currentIndex] = newNode;
        return newNode;
    }
}


/*---------------------------------------------------------------------------*/

void _shinyManager_insertNodeIntoTable(ShinyManager * self, ShinyNode * node) {

    uint32_t newNodeHash = _shinyHashValueForNodeIndex(node->parent, node->zone);
    uint32_t newNodeIndex = newNodeHash & self->_tableMask;

    if (self->_nodeTable[newNodeIndex]) {
        uint32_t step = _shinyHashValueForNodeIndexOffset(newNodeHash);

        while (self->_nodeTable[newNodeIndex]) {
            newNodeIndex = (newNodeIndex + step) & self->_tableMask;
        }
    }

    self->_nodeTable[newNodeIndex] = node;
}


/*---------------------------------------------------------------------------*/

ShinyNode * _shinyManager_createNode(
    ShinyManager * self, 
    ShinyNodeCache * nodeCache,
    ShinyZone * zone
) {
    ShinyNode * newNode = shinyNodePool_newItem(self->_lastNodePool);
    shinyNode_init(newNode, self->_currentNode, zone, nodeCache);

    self->_nodeCount++;
    _shinyManager_insertNodeIntoTable(self, newNode);
    return newNode;
}


/*---------------------------------------------------------------------------*/

void _shinyManager_createFirstNodePool(ShinyManager * self, uint32_t poolCapacity) {
    self->_firstNodePool = shinyNodePool_create(poolCapacity);
    self->_lastNodePool = self->_firstNodePool;
}


/*---------------------------------------------------------------------------*/

void _shinyManager_createAdditionalNodePool(ShinyManager * self, uint32_t additionalPoolCapacity) {
    ShinyNodePool * pPool = shinyNodePool_create(additionalPoolCapacity);
    self->_lastNodePool->nextPool = pPool;
    self->_lastNodePool = pPool;
}


/*---------------------------------------------------------------------------*/

void _shinyManager_createNodeTable(ShinyManager * self, uint32_t slotCount) {

    self->_tableSize = slotCount;
    self->_tableMask = slotCount - 1;

    self->_nodeTable = (ShinyNodeTable*) malloc(sizeof(ShinyNode*) * slotCount);

    memset(self->_nodeTable, 0, slotCount * sizeof(ShinyNode*));
}


/*---------------------------------------------------------------------------*/

void _shinyManager_resizeNodeTable(ShinyManager * self, uint32_t newSlotCount) {

    ShinyNodePool * currentNodePool;

    free(self->_nodeTable);
    _shinyManager_createNodeTable(self, newSlotCount);

    currentNodePool = self->_firstNodePool;
    while (currentNodePool) {

        ShinyNode * currentNode = shinyNodePool_firstItem(currentNodePool);

        while (currentNode != currentNodePool->_nextItem) {
            _shinyManager_insertNodeIntoTable(self, currentNode++);
        }

        currentNodePool = currentNodePool->nextPool;
    }
}


/*---------------------------------------------------------------------------*/

void shinyManager_resetZones(ShinyManager * self) {
    shinyZone_resetChain(&self->_firstZone);
    self->_lastZone = &self->_firstZone;
    self->_zoneCount = 1;
}


/*---------------------------------------------------------------------------*/

void shinyManager_destroyNodes(ShinyManager * self) {
    if (self->_firstNodePool) {
        shinyNodePool_destroy(self->_firstNodePool);
        self->_firstNodePool = NULL;
    }

    if (self->_nodeTable != _shinyManager_dummyNodeTable) {
        free(self->_nodeTable);

        self->_nodeTable = _shinyManager_dummyNodeTable;
        self->_tableSize = 1;
        self->_tableMask = 0;
    }

    self->_currentNode = &self->_rootNode;
    self->_nodeCount = 1;

    _shinyManager_init(self);
}


/*---------------------------------------------------------------------------*/

const char* shinyManager_getOutputErrorString(ShinyManager * self) {
    if (self->_firstUpdate) return "!!! Profile data must first be updated !!!";
    else if (!self->_initialized) return "!!! No profiles where executed !!!";
    else return NULL;
}


/*---------------------------------------------------------------------------*/

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4996)
#endif

int shinyManager_output(ShinyManager * self, const char * filename) {
    if (!filename) {
        shinyManager_outputToStream(self, stdout);

    } else {
        FILE *file = fopen(filename, "w");
        if (!file) return FALSE;
        shinyManager_outputToStream(self, file);
        fclose(file);
    }

    return TRUE;
}

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	pragma warning (pop)
#endif


/*---------------------------------------------------------------------------*/

void shinyManager_outputToStream(ShinyManager * self, FILE * stream) {
    const char *error = shinyManager_getOutputErrorString(self);

    if (error) {
        fwrite(error, 1, strlen(error), stream);
        fwrite("\n\n", 1, 2, stream);
        return;
    }

#if SHINY_OUTPUT_MODE & SHINY_OUTPUT_MODE_FLAT
    shinyManager_sortZones(self);

    {
        int size = shinyPrintZonesSize(self->_zoneCount);
        char *buffer = (char*) malloc(size);
        shinyPrintZones(buffer, &self->_firstZone);
        fwrite(buffer, 1, size - 1, stream);
        fwrite("\n\n", 1, 2, stream);
        free(buffer);
    }
#endif

#if SHINY_OUTPUT_MODE & SHINY_OUTPUT_MODE_TREE
    {
        int size = shinyPrintNodesSize(self->_nodeCount);
        char *buffer = (char*) malloc(size);
        shinyPrintNodes(buffer, &self->_rootNode);
        fwrite(buffer, 1, size - 1, stream);
        fwrite("\n\n", 1, 2, stream);
        free(buffer);
    }
#endif
}

#endif /* SHINY_IS_COMPILED */
