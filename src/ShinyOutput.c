// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#ifdef __cplusplus
    #include <cstdio>
#else
    #include <stdio.h>
#endif

#include "ShinyOutput.h"


#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#   pragma warning(disable: 4996)
#   define snprintf		_snprintf
#   define TRAILING		0

#else
#   define TRAILING		1
#endif



/*---------------------------------------------------------------------------*/

#define OUTPUT_WIDTH_CALL	6
#define OUTPUT_WIDTH_TIME	6
#define OUTPUT_WIDTH_PERC	4
#define OUTPUT_WIDTH_SUM	79

#define OUTPUT_WIDTH_DATA	(1+OUTPUT_WIDTH_CALL + 1 + 2*(OUTPUT_WIDTH_TIME+4+OUTPUT_WIDTH_PERC+1) + 1)
#define OUTPUT_WIDTH_NAME	(OUTPUT_WIDTH_SUM - OUTPUT_WIDTH_DATA)


/*---------------------------------------------------------------------------*/

SHINY_INLINE char * printHeader(char * output, const char * title) {
    snprintf(output, OUTPUT_WIDTH_SUM + TRAILING,
             "%-*s %*s %*s %*s",
             OUTPUT_WIDTH_NAME, title,
             OUTPUT_WIDTH_CALL, "calls",
        OUTPUT_WIDTH_TIME+4+OUTPUT_WIDTH_PERC+1, "self time",
        OUTPUT_WIDTH_TIME+4+OUTPUT_WIDTH_PERC+1, "total time");

    return output + OUTPUT_WIDTH_SUM;
}


/*---------------------------------------------------------------------------*/

SHINY_INLINE char * printData(char * output, const ShinyData * data, float toPercent) {
    float totalTicksAvg = shinyData_totalTicksAvg(data);
    const ShinyTimeUnit *selfUnit = shinyGetTimeUnit(data->selfTicks.avg);
    const ShinyTimeUnit *totalUnit = shinyGetTimeUnit(totalTicksAvg);

    snprintf(output, OUTPUT_WIDTH_DATA + TRAILING,
             " %*.1f %*.0f %-2s %*.0f%% %*.0f %-2s %*.0f%%",
             OUTPUT_WIDTH_CALL, data->entryCount.avg,
             OUTPUT_WIDTH_TIME, data->selfTicks.avg * selfUnit->invTickFreq, selfUnit->suffix,
             OUTPUT_WIDTH_PERC, data->selfTicks.avg * toPercent,
             OUTPUT_WIDTH_TIME, totalTicksAvg * totalUnit->invTickFreq, totalUnit->suffix,
             OUTPUT_WIDTH_PERC, totalTicksAvg * toPercent);

    return output + OUTPUT_WIDTH_DATA;
}


/*---------------------------------------------------------------------------*/

SHINY_INLINE char * printNode(char * output, const ShinyNode * node, float toPercent) {
    int offset = node->entryLevel * 2;

    snprintf(output, OUTPUT_WIDTH_NAME + TRAILING, "%*s%-*s",
             offset, "", OUTPUT_WIDTH_NAME - offset, node->zone->name);

    output += OUTPUT_WIDTH_NAME;

    output = printData(output, &node->data, toPercent);
    return output;
}


/*---------------------------------------------------------------------------*/

SHINY_INLINE char * printZone(char * output, const ShinyZone * zone, float toPercent) {
    snprintf(output, OUTPUT_WIDTH_NAME + TRAILING, "%-*s",
             OUTPUT_WIDTH_NAME, zone->name);

    output += OUTPUT_WIDTH_NAME;

    output = printData(output, &zone->data, toPercent);
    return output;
}


/*---------------------------------------------------------------------------*/

int shinyPrintNodesSize(uint32_t count) {
    return (1 + count) * (OUTPUT_WIDTH_SUM + 1);
}


/*---------------------------------------------------------------------------*/

int shinyPrintZonesSize(uint32_t count) {
    return (1 + count) * (OUTPUT_WIDTH_SUM + 1);
}


/*---------------------------------------------------------------------------*/

void shinyPrintANode(char * output, const ShinyNode * node, const ShinyNode * root) {
    float fTicksToPc = 100.0f / root->data.childTicks.avg;
    output = printNode(output, node, fTicksToPc);
    (*output++) = '\0';
}


/*---------------------------------------------------------------------------*/

void shinyPrintAZone(char * output, const ShinyZone * zone, const ShinyZone * root) {
    float fTicksToPc = 100.0f / root->data.childTicks.avg;
    output = printZone(output, zone, fTicksToPc);
    (*output++) = '\0';
}


/*---------------------------------------------------------------------------*/

void shinyPrintNodes(char * output, const ShinyNode * root) {
    float fTicksToPc = 100.0f / root->data.childTicks.avg;
    const ShinyNode *node = root;

    output = printHeader(output, "call tree");
    (*output++) = '\n';

    for (;;) {
        output = printNode(output, node, fTicksToPc);

        node = shinyNode_findNextInTree(node);
        if (node) {
            (*output++) = '\n';
        } else {
            (*output++) = '\0';
            return;
        }
    }
}


/*---------------------------------------------------------------------------*/

void shinyPrintZones(char * output, const ShinyZone * root) {
    float fTicksToPc = 100.0f / root->data.childTicks.avg;
    const ShinyZone *zone = root;

    output = printHeader(output, "sorted list");
    (*output++) = '\n';

    for (;;) {
        output = printZone(output, zone, fTicksToPc);

        zone = zone->next;
        if (zone) {
            (*output++) = '\n';
        } else {
            (*output++) = '\0';
            return;
        }
    }
}

#endif /* SHINY_IS_COMPILED */
