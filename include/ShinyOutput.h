#ifndef SHINY_OUTPUT_H
#define SHINY_OUTPUT_H

/* ShinyCommons.h must be included into all sources and headers as a first include */
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#include "ShinyNode.h"
#include "ShinyZone.h"


#ifdef __cplusplus
extern "C" {
#endif


SHINY_API int ShinyPrintNodesSize(uint32_t a_count);
SHINY_API int ShinyPrintZonesSize(uint32_t a_count);

SHINY_API void ShinyPrintANode(char *output, const ShinyNode *a_node, const ShinyNode *a_root);
SHINY_API void ShinyPrintAZone(char *output, const ShinyZone *a_zone, const ShinyZone *a_root);

SHINY_API void ShinyPrintNodes(char *output, const ShinyNode *a_root);
SHINY_API void ShinyPrintZones(char *output, const ShinyZone *a_root);


#ifdef __cplusplus
} // extern "C"
#endif



/* C++ API */

#ifdef __cplusplus

#include <string>

SHINY_INLINE std::string ShinyNodesToString(const ShinyNode *a_root, uint32_t a_count) {
    std::string str;
    str.resize(ShinyPrintNodesSize(a_count) - 1);
    ShinyPrintNodes(&str[0], a_root);
    return str;
}

SHINY_INLINE std::string ShinyZonesToString(const ShinyZone *a_root, uint32_t a_count) {
    std::string str;
    str.resize(ShinyPrintZonesSize(a_count) - 1);
    ShinyPrintZones(&str[0], a_root);
    return str;
}

#endif /* __cplusplus */


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_OUTPUT_H */
