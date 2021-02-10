#ifndef CONFIG_FUNCTION_H
#define CONFIG_FUNCTION_H

/* Work out where timeval lives */
# ifdef TIME_IN_SYS_RESOURCE
#  include <sys/resource.h>
# endif
# ifdef TIME_IN_SYS_TIME
#  include <sys/time.h>
# endif

/* Work out where gettimeofday lives */
# ifdef GETTIMEOFDAY_IN_SYS_TIME
#  include <sys/time.h>
# endif
# ifdef GETTIMEOFDAY_IN_TIME
#  include <time.h>
# endif

#endif /* CONFIG_FUNCTION_H */
