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
 * ShinyManager (and it's instance `Shiny_instance`) is a central object in Shiny.
 * 
 * It manages `ShinyZone` and `ShinyNode` objects. Particularly, it
 * collects all zones linked to its `rootZone`. And it creates nodes
 * as needed, assigns them to Zones and builds tree-structure out of
 * them, which denotes call-tree or inclusion-tree. This tree is attached
 * to `rootNode`.
 * 
 * As auxiliary objects `NodePool`s are managed by `ShinyManager`.
 * 
 * Also to keep track of correspondence between Node (as part of the
 * tree) and Zone Manager maintains hashtable of "parent-node"-zone
 * pairs. Memory used for it is pointed to by `_nodeTable` and methods
 * for it are implemented directly in Manager.
 * 
 * At the beginning of each Zone the method `ShinyManager_lookupAndBeginNode()`
 * is called. It creates new or finds existing node, which corresponds
 * to current Zone in context of `Shiny_instance->_curNode` at the moment
 * of invocation, makes it current and starts time recording for this node.
 * This all happens as part of macro `PROFILE_BEGIN()`
 * 
 * At the end of the Zone the method `ShinyManager_endCurNode()` is called, which
 * ends time recording in the current node. Used in macro `PROFILE_END()`
 * 
 * The function `ShinyManager_update()` calculates cumulative timing information
 * from Nodes and assigns it to Zones. Used in macro `PROFILE_UPDATE()`.
 * It should be called before outputting the profiling results.
 */

/* TODO: Reorder fields to group them according to semantics. Don't forget reorder accordingly initializer in .c file. */
typedef struct _ShinyManager {

    shinytick_t _lastTick;

    ShinyNode * _curNode;

    uint32_t _tableMask; /* = _tableSize - 1 */ /**< To calculate table index from hash. */

    ShinyNodeTable * _nodeTable; /**< Hash table of nodes. */

#if SHINY_LOOKUP_RATE == TRUE
    uint64_t _lookupCount;        /**< Counter of lookups in _nodeTable hash table. */
    uint64_t _lookupSuccessCount; /**< Counter of successful lookups in _nodesTable. */
#endif

    uint32_t _tableSize; /**< Number of slots in _nodeTable. Must be power of 2. */

    uint32_t nodeCount; /**< Number of nodes attached to the manager. */
    uint32_t zoneCount; /**< Number of zones attached to the manager. */

    ShinyZone * _lastZone; /**< End of the linked list of zones (zone->next) */

    ShinyNodePool * _lastNodePool;  /**< End of the linked list of node pools. */
    ShinyNodePool * _firstNodePool; /**< Beginning of the linked list of node pools. */

    /* rootNode and rootZone are not pointers. They are placeholders to attach actual further nodes. */
    ShinyNode rootNode; /**< First node in the linked list of nodes. */
    ShinyZone rootZone; /**< First zone in the linked list of zones. */

    float damping;

    int _initialized; /**< Is ShinyManager already initialized. */
    int _firstUpdate;
} ShinyManager;


/*---------------------------------------------------------------------------*/

/**
 * Used as initial value of _nodeTable of global ShinyManager instance.
 * By checking the equality one can see, if ShinyManager was initialized.
 */
extern ShinyNode * _ShinyManager_dummyNodeTable[];


/** Global `ShinyManager` object, which manages everything else. */
extern ShinyManager Shiny_instance;


/*---------------------------------------------------------------------------*/

SHINY_INLINE void _ShinyManager_appendTicksToCurNode(ShinyManager *self) {
    shinytick_t curTick;
    ShinyGetTicks(&curTick);

    ShinyNode_appendTicks(self->_curNode, curTick - self->_lastTick);
    self->_lastTick = curTick;
}

SHINY_API ShinyNode *_ShinyManager_lookupNode(ShinyManager *self, ShinyNodeCache *a_cache, ShinyZone *a_zone);

SHINY_API void _ShinyManager_createNodeTable(ShinyManager *self, uint32_t a_count);
SHINY_API void _ShinyManager_resizeNodeTable(ShinyManager *self, uint32_t a_count);


/** Create first node pool. Should only be called once on initialization. */
SHINY_API void _ShinyManager_createNodePool(ShinyManager *self, uint32_t a_count);

/* TODO: Rename this. */
/** Create additional node pool of size `additionalPoolCapacity`. */
SHINY_API void _ShinyManager_resizeNodePool(ShinyManager * self, uint32_t additionalPoolCapacity);


SHINY_API ShinyNode *_ShinyManager_createNode(ShinyManager *self, ShinyNodeCache *a_cache, ShinyZone *a_pZone);
SHINY_API void _ShinyManager_insertNode(ShinyManager *self, ShinyNode *a_pNode);

SHINY_INLINE void _ShinyManager_init(ShinyManager *self) {
    self->_initialized = TRUE;

    self->rootNode._last.entryCount = 1;
    self->rootNode._last.selfTicks = 0;
    ShinyGetTicks(&self->_lastTick);
}

SHINY_INLINE void _ShinyManager_uninit(ShinyManager *self) {
    self->_initialized = FALSE;

    ShinyNode_clear(&self->rootNode);
    self->rootNode.parent = &self->rootNode;
    self->rootNode.zone = &self->rootZone;
}

/* TODO: Move private functions to the .c file. */
#if SHINY_LOOKUP_RATE == TRUE
SHINY_INLINE void _ShinyManager_incLookup(ShinyManager *self) { self->_lookupCount++; }
SHINY_INLINE void _ShinyManager_incLookupSuccess(ShinyManager *self) { self->_lookupSuccessCount++; }
SHINY_INLINE float ShinyManager_lookupRate(const ShinyManager *self) { return ((float) self->_lookupSuccessCount) / ((float) self->_lookupCount); }
#else
SHINY_INLINE void _ShinyManager_incLookup(ShinyManager *self) {}
SHINY_INLINE void _ShinyManager_incLookupSuccess(ShinyManager *self) {}
SHINY_INLINE float ShinyManager_lookupRate(const ShinyManager *self) { return -1; }
#endif

SHINY_API void ShinyManager_resetZones(ShinyManager *self);
SHINY_API void ShinyManager_destroyNodes(ShinyManager *self);

SHINY_INLINE float ShinyManager_tableUsage(const ShinyManager *self) {
    return ((float) self->nodeCount) / ((float) self->_tableSize);
}

SHINY_INLINE uint32_t ShinyManager_allocMemInBytes(const ShinyManager *self) {
    return self->_tableSize * sizeof(ShinyNode *)
           + (self->_firstNodePool) ? ShinyNodePool_memoryUsageChain(self->_firstNodePool) : 0;
}

SHINY_INLINE void ShinyManager_beginNode(ShinyManager * self, ShinyNode * node) {
    ShinyNode_beginEntry(node);

    _ShinyManager_appendTicksToCurNode(self);
    self->_curNode = node;
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
SHINY_INLINE void ShinyManager_lookupAndBeginNode(
    ShinyManager * self, 
    ShinyNodeCache * nodeCache, 
    ShinyZone * zone
) {

    if (self->_curNode != (*nodeCache)->parent) {
        *nodeCache = _ShinyManager_lookupNode(self, nodeCache, zone);
    }

    ShinyManager_beginNode(self, *nodeCache);
}

SHINY_INLINE void ShinyManager_endCurNode(ShinyManager *self) {

    _ShinyManager_appendTicksToCurNode(self);
    self->_curNode = self->_curNode->parent;
}

/**/

SHINY_API void ShinyManager_preLoad(ShinyManager *self);

SHINY_API void ShinyManager_updateClean(ShinyManager *self);
SHINY_API void ShinyManager_update(ShinyManager *self);

SHINY_API void ShinyManager_clear(ShinyManager *self);
SHINY_API void ShinyManager_destroy(ShinyManager *self);

SHINY_INLINE void ShinyManager_sortZones(ShinyManager *self) {
    if (self->rootZone.next)
        self->_lastZone = ShinyZone_sortChain(&self->rootZone.next);
}

SHINY_API const char *ShinyManager_getOutputErrorString(ShinyManager *self);

SHINY_API int ShinyManager_output(ShinyManager *self, const char *a_filename);
SHINY_API void ShinyManager_outputToStream(ShinyManager *self, FILE *stream);


SHINY_INLINE int ShinyManager_isZoneSelfTimeBelow(ShinyManager *self, ShinyZone* a_zone, float a_percentage) {
    return a_percentage * (float) self->rootZone.data.childTicks.cur
           <= (float) a_zone->data.selfTicks.cur;
}

SHINY_INLINE int ShinyManager_isZoneTotalTimeBelow(ShinyManager *self, ShinyZone* a_zone, float a_percentage) {
    return a_percentage * (float) self->rootZone.data.childTicks.cur
           <= (float) ShinyData_totalTicksCur(&a_zone->data);
}

/**/

SHINY_INLINE void ShinyManager_enumerateNodes(ShinyManager *self, void (*a_func)(const ShinyNode*)) {
    ShinyNode_enumerateNodes(&self->rootNode, a_func);
}

SHINY_INLINE void ShinyManager_enumerateZones(ShinyManager *self, void (*a_func)(const ShinyZone*)) {
    ShinyZone_enumerateZones(&self->rootZone, a_func);
}

#ifdef __cplusplus
} // extern "C"
#endif



/* C++ API */

#if __cplusplus

SHINY_INLINE std::string ShinyManager_outputTreeToString(ShinyManager *self) {
    const char* error = ShinyManager_getOutputErrorString(self);
    if (error) return error;
    else return ShinyNodesToString(&self->rootNode, self->nodeCount);
}

SHINY_INLINE std::string ShinyManager_outputFlatToString(ShinyManager *self) {
    const char* error = ShinyManager_getOutputErrorString(self);
    if (error) return error;

    ShinyManager_sortZones(self);
    return ShinyZonesToString(&self->rootZone, self->zoneCount);
}

template <class T> void ShinyManager_enumerateNodes(ShinyManager *self, T* a_this, void (T::*a_func)(const ShinyNode*)) {
    ShinyNode_enumerateNodes(&self->rootNode, a_this, a_func);
}

template <class T> void ShinyManager_enumerateZones(ShinyManager *self, T* a_this, void (T::*a_func)(const ShinyZone*)) {
    ShinyZone_enumerateZones(&self->rootZone, a_this, a_func);
}

class ShinyEndNodeOnDestruction {
public:

    inline ~ShinyEndNodeOnDestruction() {
        ShinyManager_endCurNode(&Shiny_instance);
    }
};

#endif /* __cplusplus */


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_MANAGER_H */
