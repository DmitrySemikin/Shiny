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


SHINY_API int shinyPrintNodesSize(uint32_t count);
SHINY_API int shinyPrintZonesSize(uint32_t count);

SHINY_API void shinyPrintANode(char * output, const ShinyNode * node, const ShinyNode * root);
SHINY_API void shinyPrintAZone(char * output, const ShinyZone * zone, const ShinyZone * root);

SHINY_API void shinyPrintNodes(char * output, const ShinyNode * root);
SHINY_API void shinyPrintZones(char * output, const ShinyZone * root);


#ifdef __cplusplus
} // extern "C"
#endif



/* C++ API */

#ifdef __cplusplus

#include <string>

SHINY_INLINE std::string shinyNodesToString(const ShinyNode *a_root, uint32_t a_count) {
    std::string str;
    str.resize(shinyPrintNodesSize(a_count) - 1);
    shinyPrintNodes(&str[0], a_root);
    return str;
}

SHINY_INLINE std::string shinyZonesToString(const ShinyZone *a_root, uint32_t a_count) {
    std::string str;
    str.resize(shinyPrintZonesSize(a_count) - 1);
    shinyPrintZones(&str[0], a_root);
    return str;
}

#endif /* __cplusplus */


#endif /* SHINY_IS_COMPILED */

#endif /* SHINY_OUTPUT_H */
