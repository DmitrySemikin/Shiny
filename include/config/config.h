#ifndef CONFIG_H
#define CONFIG_H

/* Include the configuration knobs: these must be included first
** as they may affect the behaviour of the following configure files
**/
#include "config_knobs.h"

/* Include the platform checks from CMake */
#include "config_cmake.h"

/* Include the platform specific headers */
#include "config_header.h"

/* Include the headers for the needed function prototypes */
#include "config_function.h"

/* Include the library defines */
#include "config_define.h"

#endif /* CONFIG_H */
