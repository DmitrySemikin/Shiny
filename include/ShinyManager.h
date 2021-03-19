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


typedef struct {

    shinytick_t _lastTick;

    ShinyNode *_curNode;

    uint32_t _tableMask; /* = _tableSize - 1 */

    ShinyNodeTable *_nodeTable;

#if SHINY_LOOKUP_RATE == TRUE
    uint64_t _lookupCount;
    uint64_t _lookupSuccessCount;
#endif

    uint32_t _tableSize;

    uint32_t nodeCount;
    uint32_t zoneCount;

    ShinyZone *_lastZone;

    ShinyNodePool *_lastNodePool;
    ShinyNodePool *_firstNodePool;

    ShinyNode rootNode;
    ShinyZone rootZone;

    float damping;

    int _initialized;
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

SHINY_INLINE void ShinyManager_beginNode(ShinyManager *self, ShinyNode *a_node) {
    ShinyNode_beginEntry(a_node);

    _ShinyManager_appendTicksToCurNode(self);
    self->_curNode = a_node;
}

SHINY_INLINE void ShinyManager_lookupAndBeginNode(ShinyManager *self, ShinyNodeCache *a_cache, ShinyZone *a_zone) {

    if (self->_curNode != (*a_cache)->parent)
        *a_cache = _ShinyManager_lookupNode(self, a_cache, a_zone);

    ShinyManager_beginNode(self, *a_cache);
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
