cmake_minimum_required(VERSION 3.13)
project(stduuid CXX)

set(UUID_SYSTEM_GENERATOR ON)
option(UUID_SYSTEM_GENERATOR "Enable operating system uuid generator" ON)
option(UUID_TIME_GENERATOR "Enable experimental time-based uuid generator" OFF)
option(UUID_USING_CXX20_SPAN "Using span from std instead of gsl" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Library target
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Using system uuid generator
if (UUID_SYSTEM_GENERATOR)
    target_compile_definitions(${PROJECT_NAME} INTERFACE UUID_SYSTEM_GENERATOR)

    if (WIN32)
    elseif (APPLE)
        find_library(CFLIB CoreFoundation REQUIRED)
        target_link_libraries(${PROJECT_NAME} INTERFACE ${CFLIB})
    else ()
        find_package(Libuuid REQUIRED)
        if (Libuuid_FOUND)
            target_include_directories(${PROJECT_NAME} INTERFACE ${Libuuid_INCLUDE_DIRS})
            target_link_libraries(${PROJECT_NAME} INTERFACE ${Libuuid_LIBRARIES})
        endif ()
    endif ()
endif ()

# Using time-based generator
if (UUID_TIME_GENERATOR)
    target_compile_definitions(${PROJECT_NAME} INTERFACE UUID_TIME_GENERATOR)
endif ()

# Using span from std
if (NOT UUID_USING_CXX20_SPAN)
    target_include_directories(${PROJECT_NAME} INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gsl>)
endif ()