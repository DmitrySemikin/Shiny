#ifndef SHINY_MANAGER_H
#define SHINY_MANAGER_H

/* ShinyCommons.h must be included into all sources and headers as a first include */
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#ifdef __cplusplus
    #include <cstdio>
#else
    #include <stdio.h>
#endif


#include "ShinyZone.h"
#include "ShinyNode.h"
#include "ShinyNodePool.h"
#include "ShinyTools.h"
#include "ShinyOutput.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * ShinyManager (and it's instance `shinyManagerInstance`) is a central object in Shiny.
 * 
 * It manages `ShinyZone` and `ShinyNode` objects. Particularly, it
 * collects all zones linked to its `_firstZone`. And it creates nodes
 * as needed, assigns them to Zones and builds tree-structure out of
 * them, which denotes call-tree or inclusion-tree. This tree is attached
 * to `_rootNode`.
 * 
 * As auxiliary objects `NodePool`s are managed by `ShinyManager`.
 * 
 * Also to keep track of correspondence between Node (as part of the
 * tree) and Zone Manager maintains hashtable of "parent-node"-zone
 * pairs. Memory used for it is pointed to by `_nodeTable` and methods
 * for it are implemented directly in Manager.
 * 
 * At the beginning of each Zone the method `shinyManager_lookupAndBeginNode()`
 * is called. It creates new or finds existing node, which corresponds
 * to current Zone in context of `shinyManagerInstance->_currentNode` at the moment
 * of invocation, makes it current and starts time recording for this node.
 * This all happens as part of macro `PROFILE_BEGIN()`
 * 
 * At the end of the Zone the method `shinyManager_endCurrentNode()` is called, which
 * ends time recording in the current node. Used in macro `PROFILE_END()`
 * 
 * The function `shinyManager_update()` calculates cumulative timing information
 * from Nodes and assigns it to Zones. Used in macro `PROFILE_UPDATE()`.
 * It should be called before outputting the profiling results.
 */
typedef struct ShinyManager {

    /* Note: We don't really care about alignment. We only have one object of this type. */

    shinytick_t _lastTick;       /** Wall clock we last noticed (in ticks). Used to calculate time differences. */

    ShinyNode * _currentNode;    /**< Well... it is current node. It will be context for any Zone, which execution meets next. */

    ShinyNode _rootNode;         /**< Root (dummy) node in the tree of nodes. Not pointer. */

    ShinyZone _firstZone;        /**< First (dummy) zone in the linked list of zones. Not pointer. */
    ShinyZone * _lastZone;       /**< End of the linked list of zones (zone->next), which starts with `_firstZone`. */

    uint32_t _nodeCount;         /**< Number of nodes attached to the manager. */
    uint32_t _zoneCount;         /**< Number of zones attached to the manager. */

    ShinyNodeTable * _nodeTable; /**< Hash table of nodes. Hash of node is calculated from its Zone and parentNode. */
    uint32_t _tableSize;         /**< Number of slots in _nodeTable. Must be power of 2. */
    uint32_t _tableMask;         /**< (= _tableSize - 1) To calculate table index from hash. */

    ShinyNodePool * _firstNodePool; /**< Beginning of the linked list of node pools. */
    ShinyNodePool * _lastNodePool;  /**< End of the linked list of node pools. */

    float damping;               /**< Used when calling `update()` repeatedly. */

    int _initialized;            /**< Is ShinyManager already initialized: 0 for FALSE, 1 for TRUE. */
    int _firstUpdate;            /**< Is it first call of `shinyManager_update()`: 0 for FALSE, 1 for TRUE. */

#if SHINY_LOOKUP_RATE == TRUE
    uint64_t _lookupCount;        /**< Counter of lookups in _nodeTable hash table. */
    uint64_t _lookupSuccessCount; /**< Counter of successful lookups in _nodesTable. */
#endif

} ShinyManager;


/*---------------------------------------------------------------------------*/

/**
 * Used as initial value of _nodeTable of global ShinyManager instance.
 * By checking the equality one can see, if ShinyManager was initialized.
 */
extern ShinyNode * _shinyManager_dummyNodeTable[];


/** Global `ShinyManager` object, which manages everything else. */
extern ShinyManager shinyManagerInstance;


/*---------------------------------------------------------------------------*/

SHINY_INLINE void _shinyManager_appendTicksToCurNode(ShinyManager *self) {
    shinytick_t curTick;
    ShinyGetTicks(&curTick);

    shinyNode_appendTicks(self->_currentNode, curTick - self->_lastTick);
    self->_lastTick = curTick;
}

SHINY_API ShinyNode * _shinyManager_lookupNode(ShinyManager * self, ShinyNodeCache * nodeCache, ShinyZone * zone);

/** @param slotCount  Number of slots in the created table. Must be power of 2. */
SHINY_API void _shinyManager_createNodeTable(ShinyManager * self, uint32_t slotCount);
SHINY_API void _shinyManager_resizeNodeTable(ShinyManager * self, uint32_t newSlotCount);


/** Create first node pool. Should only be called once on initialization. */
SHINY_API void _shinyManager_createFirstNodePool(ShinyManager * self, uint32_t poolCapacity);

/** Create additional node pool of size `additionalPoolCapacity`. */
SHINY_API void _shinyManager_createAdditionalNodePool(ShinyManager * self, uint32_t additionalPoolCapacity);


SHINY_API ShinyNode * _shinyManager_createNode(ShinyManager * self, ShinyNodeCache * nodeCache, ShinyZone * zone);
SHINY_API void _shinyManager_insertNodeIntoTable(ShinyManager * self, ShinyNode * node);

SHINY_INLINE void _shinyManager_init(ShinyManager * self) {
    self->_initialized = TRUE;

    self->_rootNode._last.entryCount = 1;
    self->_rootNode._last.selfTicks = 0;
    ShinyGetTicks(&self->_lastTick);
}

SHINY_INLINE void _shinyManager_uninit(ShinyManager * self) {
    self->_initialized = FALSE;

    shinyNode_clear(&self->_rootNode);
    self->_rootNode.parent = &self->_rootNode;
    self->_rootNode.zone = &self->_firstZone;
}

#if SHINY_LOOKUP_RATE == TRUE
SHINY_INLINE void _shinyManager_incLookup(ShinyManager * self) { self->_lookupCount++; }
SHINY_INLINE void _shinyManager_incLookupSuccess(ShinyManager * self) { self->_lookupSuccessCount++; }
SHINY_INLINE float shinyManager_lookupRate(const ShinyManager * self) { return ((float) self->_lookupSuccessCount) / ((float) self->_lookupCount); }
#else
SHINY_INLINE void _shinyManager_incLookup(ShinyManager * self) {}
SHINY_INLINE void _shinyManager_incLookupSuccess(ShinyManager * self) {}
SHINY_INLINE float shinyManager_lookupRate(const ShinyManager * self) { return -1; }
#endif

SHINY_API void shinyManager_resetZones(ShinyManager * self);
SHINY_API void shinyManager_destroyNodes(ShinyManager * self);

SHINY_INLINE float shinyManager_tableUsage(const ShinyManager * self) {
    return ((float) self->_nodeCount) / ((float) self->_tableSize);
}

SHINY_INLINE uint32_t shinyManager_allocMemInBytes(const ShinyManager * self) {
    return self->_tableSize * sizeof(ShinyNode *)
           + ((self->_firstNodePool) ? ShinyNodePool_memoryUsageChain(self->_firstNodePool) : 0);
}

SHINY_INLINE void shinyManager_beginNode(ShinyManager * self, ShinyNode * node) {
    shinyNode_beginEntry(node);

    _shinyManager_appendTicksToCurNode(self);
    self->_currentNode = node;
}

/** This function is to be called at the beginning of the zone.
 * 
 * It will check, if nodeCache alrady has corresponding Node,
 * i.e. the node for given Zone, which is created as child of
 * `self->curNode`.
 * 
 * If yes, then it will just use this node, but if not, then it
 * will try to lookup the node in its' node table and finally, if
 * it will not be found, then it will create it.
 * 
 * Found or created node will be stored in the cache.
 * 
 * Note, that if execution hits the same zone in the same context
 * again and again, then cache will be used and no table lookup
 * is needed. But if the zone is hit from different context, then
 * cache will contain wrong node, so the manager will need to look
 * up proper node each time the context was changed.
 * 
 */
SHINY_INLINE void shinyManager_lookupAndBeginNode(
    ShinyManager * self, 
    ShinyNodeCache * nodeCache, 
    ShinyZone * zone
) {

    if (self->_currentNode != (*nodeCache)->parent) {
        *nodeCache = _shinyManager_lookupNode(self, nodeCache, zone);
    }

    shinyManager_beginNode(self, *nodeCache);
}

SHINY_INLINE void shinyManager_endCurrentNode(ShinyManager * self) {

    _shinyManager_appendTicksToCurNode(self);
    self->_currentNode = self->_currentNode->parent;
}

/**/

SHINY_API void shinyManager_preLoad(ShinyManager * self);

SHINY_API void shinyManager_updateClean(ShinyManager * self);
SHINY_API void shinyManager_update(ShinyManager * self);

SHINY_API void shinyManager_clear(ShinyManager * self);
SHINY_API void shinyManager_destroy(ShinyManager * self);

SHINY_INLINE void shinyManager_sortZones(ShinyManager * self) {
    if (self->_firstZone.next) {
        self->_lastZone = ShinyZone_sortChain(&self->_firstZone.next);
    }
}

SHINY_API const char * shinyManager_getOutputErrorString(ShinyManager * self);

SHINY_API int shinyManager_output(ShinyManager * self, const char * filename);
SHINY_API void shinyManager_outputToStream(ShinyManager * self, FILE * stream);


SHINY_INLINE int shinyManager_isZoneSelfTimeBelow(ShinyManager * self, ShinyZone * zone, float percentage) {
    return percentage * (float) self->_firstZone.data.childTicks.cur <= (float) zone->data.selfTicks.cur;
}

SHINY_INLINE int shinyManager_isZoneTotalTimeBelow(ShinyManager * self, ShinyZone * zone, float percentage) {
    return percentage * (float) self->_firstZone.data.childTicks.cur
           <= (float) shinyData_totalTicksCur(&zone->data);
}

/**/

/** Call given function on each Node. */
SHINY_INLINE void shinyManager_enumerateNodes(ShinyManager * self, void (*functionToCall)(const ShinyNode*)) {
    shinyNode_enumerateNodes(&self->_rootNode, functionToCall);
}

/** Call given function on each Zone. */
SHINY_INLINE void shinyManager_enumerateZones(ShinyManager * self, void (*functionToCall)(const ShinyZone*)) {
    ShinyZone_enumerateZones(&self->_firstZone, functionToCall);
}

#ifdef __cplusplus
} // extern "C"
#endif



/* C++ API */

#if __cplusplus

SHINY_INLINE std::string shinyManager_outputTreeToString(ShinyManager * self) {
    const char* error = shinyManager_getOutputErrorString(self);
    if (error) return error;
    else return shinyNodesToString(&self->_rootNode, self->_nodeCount);
}

SHINY_INLINE std::string shinyManager_outputFlatToString(ShinyManager * self) {
    const char* error = shinyManager_getOutputErrorString(self);
    if (error) return error;

    shinyManager_sortZones(self);
    return shinyZonesToString(&self->_firstZone, self->_zoneCount);
}

/** call given method on given targetInstance passing as argument this node, and recursively all its subtree and subtrees of its siblings. */
template <class T> void shinyManager_enumerateNodes(
        ShinyManager * self,
        T * targetInstance,
        void (T::*methodToCall)(const ShinyNode*)
) {
    shinyNode_enumerateNodes(&self->_rootNode, targetInstance, methodToCall);
}

/** Call given method on given targetInstance passing as argument this zone and then all zones linked to it. */
template <class T> void shinyManager_enumerateZones(
        ShinyManager * self,
        T * targetInstance,
        void (T::*methodToCall)(const ShinyZone*)
) {
    ShinyZone_enumerateZones(&self->_firstZone, targetInstance, methodToCall);
}

class ShinyEndNodeOnDestruction {
public:

    inline ~ShinyEndNodeOnDestruction() {
        shinyManager_endCurrentNode(&shinyManagerInstance);
    }
};

#endif /* __cplusplus */


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_MANAGER_H */
