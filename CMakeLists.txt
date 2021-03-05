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
include ( CheckIncludeFiles )
include ( CheckTypeSize )
include ( cmake/extra/CheckPrototypeDefinition.cmake )


#=============================================================================

## Definition of the variables for config_cmake.h (generated from config_cmake.h.in)

if ( MSVC OR MSVC_IDE OR MSVC60 OR MSVC70 OR MSVC71 OR MSVC80 OR CMAKE_COMPILER_2005 OR MSVC90 OR MSVC10 )
    # Define the standard integer types
    if ( CYGWIN )
        set ( UINT32_T  "u_int32_t" )
        set ( UINT64_T  "u_int64_t" )
    else ( CYGWIN )
        set ( INT32_T   "int"                 )
        set ( UINT32_T  "unsigned int"        )
        set ( INT64_T   "__int64"             )
        set ( UINT64_T  "unsigned __int64"    )
    endif ( CYGWIN )
endif ( MSVC OR MSVC_IDE OR MSVC60 OR MSVC70 OR MSVC71 OR MSVC80 OR CMAKE_COMPILER_2005 OR MSVC90 OR MSVC10 )

if ( ${CMAKE_COMPILER_IS_GNUCC} )
    message ( "-- Using GCC compiler profile...")
    # Nothing to do here for now
endif ( ${CMAKE_COMPILER_IS_GNUCC} )

# Check for CLang (this should be redundant eventually, as a patch
# has been made to the 2.8 branch. However, until that version becomes
# common, this will do)
#
# NOTE: This MUST be done AFTER the GCC check, as Clang also shows up
# as GCC. So test for that first, then Clang. One day this will all go
# away..
if ( "${CMAKE_C_COMPILER_ID}" MATCHES "Clang" )
    message ( "-- Using Clang compiler profile...")
    # Nothing to do here for now
endif ( "${CMAKE_C_COMPILER_ID}" MATCHES "Clang" )


#=============================================================================


##
## Project Configuration
##


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
## Core Configuration File
##

# Set the global configure file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/include/config/config_cmake.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/config/config_cmake.h)

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
