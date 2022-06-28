# For target_compile_features
cmake_minimum_required(VERSION 3.9)

project(platform_folders VERSION 4.2.0 LANGUAGES CXX)

add_library(platform_folders STATIC
        sago/platform_folders.cpp
        )
set_property(TARGET platform_folders PROPERTY POSITION_INDEPENDENT_CODE ON)

# set_target_properties(platform_folders PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

# Creates an alias so that people building in-tree (instead of using find_package)...
# can still link against the same target
add_library(sago::platform_folders ALIAS platform_folders)

# Where to search for the header while building
target_include_directories(platform_folders PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sago>
        )

# Define the header as public for installation
set_target_properties(platform_folders PROPERTIES
        PUBLIC_HEADER "sago/platform_folders.h"
        )

# cxx_std_11 requires v3.8
if (CMAKE_VERSION VERSION_LESS "3.8.0")
    # Use old method of forcing C++11
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED true)
else ()
    # Require (minimum) C++11 when using header
    # PRIVATE means only at compile time
    target_compile_features(platform_folders PUBLIC cxx_std_11)
endif ()

# cxx_nullptr exists in v3.1
target_compile_features(platform_folders PRIVATE cxx_nullptr)

# Cmake's find_package search path is different based on the system
# See https://cmake.org/cmake/help/latest/command/find_package.html for the list
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # You can pass these when calling Cmake, so don't override if the user does
    if (NOT _WIN32_WINNT AND NOT WINVER)
        target_compile_definitions(platform_folders PRIVATE
                _WIN32_WINNT=0x0601
                WINVER=0x0601
                )
    endif ()
endif ()
