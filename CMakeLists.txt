# We want to use CMake version, which is by default available on still
# supported CentOS (currently CentOS 7 and it supports CMake 2.8)
cmake_minimum_required ( VERSION 2.8 )

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0054 NEW)

#=============================================================================


project (
    das-shiny
        VERSION      "3.0.0"
        DESCRIPTION  "C/C++ profiler with manual instrumentation. Based on original Shiny profiler by Aidin Abedi and David Love."
        HOMEPAGE_URL "https://github.com/DmitrySemikin/das-shiny"
        LANGUAGES    C CXX
)


#=============================================================================

include ( CheckLibraryExists )

#=============================================================================

##
## Library Configuration
##

# Look for the math library: on some platforms this is part of the C
# library, on others it needs linking separately
check_library_exists ( m floor "" SEPARATE_LIB_M )

if ( SEPARATE_LIB_M )
    # If the math library is separate, then it needs to be included
    # in the global link flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lm")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -lm")
endif( SEPARATE_LIB_M )

##
## C Library Sources
##

# Add the Shiny header files to the search path
include_directories (include)

add_library (
    ${PROJECT_NAME}
        src/ShinyManager.c
        src/ShinyNode.c
        src/ShinyNodePool.c
        src/ShinyNodeState.c
        src/ShinyOutput.c
        src/ShinyTools.c
        src/ShinyZone.c
)

add_subdirectory(examples)
