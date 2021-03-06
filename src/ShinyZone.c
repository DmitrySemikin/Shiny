// ShinyCommons.h must be included into all sources and headers as a first include
#include "ShinyCommons.h"

#if SHINY_IS_COMPILED


#ifdef __cplusplus
    #include <cstring>
#else
    #include <string.h>
#endif


#include "ShinyZone.h"


/*---------------------------------------------------------------------------*/

void shinyZone_preUpdateChain(ShinyZone * startZone) {
    ShinyZone * zone = startZone;

    while (zone) {
        shinyData_clearCurrent(&(zone->data));
        zone = zone->next;
    }
}


/*---------------------------------------------------------------------------*/

void shinyZone_updateChain(ShinyZone * startZone, float damping) {
    ShinyZone * zone = startZone;

    do {
        shinyData_computeAverage(&(zone->data), damping);
        zone = zone->next;
    } while (zone);
}


/*---------------------------------------------------------------------------*/

void shinyZone_updateChainClean(ShinyZone * startZone) {
    ShinyZone * zone = startZone;

    do {
        shinyData_copyAverage(&(zone->data));
        zone = zone->next;
    } while (zone);
}


/*---------------------------------------------------------------------------*/

void shinyZone_resetChain(ShinyZone * startZone) {
    ShinyZone * zone = startZone;
    ShinyZone * temp;

    do {
        zone->zoneState = SHINY_ZONE_STATE_HIDDEN;
        temp = zone->next;
        zone->next = NULL;
        zone = temp;
    } while (zone);
}

/*---------------------------------------------------------------------------*/

/* A Linked-List Memory Sort
   by Philip J. Erdelsky
   pje@efgh.com
   http://www.alumni.caltech.edu/~pje/

   Modified by Aidin Abedi
*/

ShinyZone * shinyZone_sortChain(ShinyZone ** startZone) /* return ptr to last zone */
{
    ShinyZone * p = *startZone;

    unsigned base;
    unsigned long block_size;

    struct tape
    {
        ShinyZone *first, *last;
        unsigned long count;
    } tape[4];

    /* Distribute the records alternately to tape[0] and tape[1]. */

    tape[0].count = tape[1].count = 0L;
    tape[0].first = NULL;
    base = 0;
    while (p != NULL)
    {
        ShinyZone *next = p->next;
        p->next = tape[base].first;
        tape[base].first = p;
        tape[base].count++;
        p = next;
        base ^= 1;
    }

    /* If the list is empty or contains only a single record, then */
    /* tape[1].count == 0L and this part is vacuous.               */

    for (base = 0, block_size = 1L; tape[base+1].count != 0L;
        base ^= 2, block_size <<= 1)
    {
        int dest;
        struct tape *tape0, *tape1;
        tape0 = tape + base;
        tape1 = tape + base + 1;
        dest = base ^ 2;
        tape[dest].count = tape[dest+1].count = 0;
        for (; tape0->count != 0; dest ^= 1)
        {
            unsigned long n0, n1;
            struct tape *output_tape = tape + dest;
            n0 = n1 = block_size;
            while (1)
            {
                ShinyZone *chosen_record;
                struct tape *chosen_tape;
                if (n0 == 0 || tape0->count == 0)
                {
                    if (n1 == 0 || tape1->count == 0)
                        break;
                    chosen_tape = tape1;
                    n1--;
                }
                else if (n1 == 0 || tape1->count == 0)
                {
                    chosen_tape = tape0;
                    n0--;
                }
                else if (shinyZone_compare(tape1->first, tape0->first) > 0)
                {
                    chosen_tape = tape1;
                    n1--;
                }
                else
                {
                    chosen_tape = tape0;
                    n0--;
                }
                chosen_tape->count--;
                chosen_record = chosen_tape->first;
                chosen_tape->first = chosen_record->next;
                if (output_tape->count == 0)
                    output_tape->first = chosen_record;
                else
                    output_tape->last->next = chosen_record;
                output_tape->last = chosen_record;
                output_tape->count++;
            }
        }
    }

    if (tape[base].count > 1L) {
        ShinyZone* last = tape[base].last;
        *startZone = tape[base].first;
        last->next = NULL;
        return last;

    } else {
        return NULL;
    }
}


/*---------------------------------------------------------------------------*/

void shinyZone_clear(ShinyZone * self) {
    memset(self, 0, sizeof(ShinyZone));
}


/*---------------------------------------------------------------------------*/

void shinyZone_enumerateZones(const ShinyZone * startZone, void (*functionToApply)(const ShinyZone*)) {
    functionToApply(startZone);

    if (startZone->next) {
        shinyZone_enumerateZones(startZone->next, functionToApply);
    }
}

#endif /* SHINY_IS_COMPILED */
