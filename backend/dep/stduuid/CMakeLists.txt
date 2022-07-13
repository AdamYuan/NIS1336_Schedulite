cmake_minimum_required(VERSION 3.7.0)
project(stduuid CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(UUID_TIME_GENERATOR OFF)
set(UUID_USING_CXX20_SPAN OFF)

# Library target
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Using time-based generator
if (UUID_TIME_GENERATOR)
    target_compile_definitions(${PROJECT_NAME} INTERFACE UUID_TIME_GENERATOR)
endif()

# Using span from std
if (NOT UUID_USING_CXX20_SPAN)
    target_include_directories(${PROJECT_NAME} INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gsl>)
endif ()
