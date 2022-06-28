#[===================================================================[
   date library by Howard Hinnant

   CMake projects that wish to use this library should consider
   something like the following :

     include( FetchContent )
     FetchContent_Declare( date_src
       GIT_REPOSITORY https://github.com/HowardHinnant/date.git
       GIT_TAG        v3.0.0  # adjust tag/branch/commit as needed
     )
     FetchContent_MakeAvailable(date_src)
     ...
     target_link_libraries (my_target PRIVATE date::date)

#]===================================================================]

cmake_minimum_required(VERSION 3.9)

project(date VERSION 3.0.0)
set(ABI_VERSION 3) # used as SOVERSION, increment when ABI changes

get_directory_property(has_parent PARENT_DIRECTORY)

# Override by setting on CMake command line.
set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")

set(USE_SYSTEM_TZ_DB ON)
set(MANUAL_TZ_DB OFF)
set(USE_TZ_DB_IN_DOT OFF)
set(BUILD_SHARED_LIBS OFF)
set(DISABLE_STRING_VIEW OFF)
set(COMPILE_WITH_C_LOCALE ON)
set(BUILD_TZ_LIB ON)

option(USE_SYSTEM_TZ_DB "Use the operating system's timezone database" OFF)
option(MANUAL_TZ_DB "User will set TZ DB manually by invoking set_install in their code" OFF)
option(USE_TZ_DB_IN_DOT "Save the timezone database in the current folder" OFF)
option(BUILD_SHARED_LIBS "Build a shared version of library" OFF)
option(DISABLE_STRING_VIEW "Disable string view" OFF)
option(COMPILE_WITH_C_LOCALE "define ONLY_C_LOCALE=1" OFF)
option(BUILD_TZ_LIB "build/install of TZ library" OFF)

function(print_option OPT)
    if (NOT DEFINED PRINT_OPTION_CURR_${OPT} OR (NOT PRINT_OPTION_CURR_${OPT} STREQUAL ${OPT}))
        set(PRINT_OPTION_CURR_${OPT} ${${OPT}} CACHE BOOL "")
        mark_as_advanced(PRINT_OPTION_CURR_${OPT})
        message("# date: ${OPT} ${${OPT}}")
    endif ()
endfunction()

print_option(USE_SYSTEM_TZ_DB)
print_option(MANUAL_TZ_DB)
print_option(USE_TZ_DB_IN_DOT)
print_option(BUILD_SHARED_LIBS)
print_option(DISABLE_STRING_VIEW)

#[===================================================================[
   date (header only) library
#]===================================================================]
add_library(date INTERFACE)
add_library(date::date ALIAS date)
target_include_directories(date INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
# adding header sources just helps IDEs
target_sources(date INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:include>/date/date.h
        # the rest of these are not currently part of the public interface of the library:
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/date/solar_hijri.h>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/date/islamic.h>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/date/iso_week.h>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/date/julian.h>
        )
if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.15)
    # public headers will get installed:
    set_target_properties(date PROPERTIES PUBLIC_HEADER include/date/date.h)
endif ()

# These used to be set with generator expressions,
#
#   ONLY_C_LOCALE=$<IF:$<BOOL:${COMPILE_WITH_C_LOCALE}>,1,0>
#
# which expand in the output target file to, e.g.
#
#   ONLY_C_LOCALE=$<IF:$<BOOL:FALSE>,1,0>
#
# This string is then (somtimes?) not correctly interpreted.
if (COMPILE_WITH_C_LOCALE)
    # To workaround libstdc++ issue https://github.com/HowardHinnant/date/issues/388
    target_compile_definitions(date INTERFACE ONLY_C_LOCALE=1)
else ()
    target_compile_definitions(date INTERFACE ONLY_C_LOCALE=0)
endif ()
if (DISABLE_STRING_VIEW)
    target_compile_definitions(date INTERFACE HAS_STRING_VIEW=0 -DHAS_DEDUCTION_GUIDES=0)
endif ()

#[===================================================================[
   tz (compiled) library
#]===================================================================]
if (BUILD_TZ_LIB)
    add_library(date-tz)
    target_sources(date-tz
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:include>/date/tz.h
            PRIVATE
            include/date/tz_private.h
            src/tz.cpp)
    if (IOS)
        target_sources(date-tz
                PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:include>/date/ios.h
                PRIVATE
                src/ios.mm)
    endif ()
    add_library(date::date-tz ALIAS date-tz)
    target_link_libraries(date-tz PUBLIC date)
    target_include_directories(date-tz PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)

    if (USE_SYSTEM_TZ_DB OR MANUAL_TZ_DB)
        target_compile_definitions(date-tz PRIVATE AUTO_DOWNLOAD=0 HAS_REMOTE_API=0)
    else ()
        target_compile_definitions(date-tz PRIVATE AUTO_DOWNLOAD=1 HAS_REMOTE_API=1)
    endif ()

    if (USE_SYSTEM_TZ_DB AND NOT WIN32 AND NOT MANUAL_TZ_DB)
        target_compile_definitions(date-tz PRIVATE INSTALL=. PUBLIC USE_OS_TZDB=1)
    else ()
        target_compile_definitions(date-tz PUBLIC USE_OS_TZDB=0)
    endif ()

    if (WIN32 AND BUILD_SHARED_LIBS)
        target_compile_definitions(date-tz PUBLIC DATE_BUILD_DLL=1)
    endif ()

    set(TZ_HEADERS include/date/tz.h)

    if (IOS)
        list(APPEND TZ_HEADERS include/date/ios.h)
    endif ()
    set_target_properties(date-tz PROPERTIES
            POSITION_INDEPENDENT_CODE ON
            PUBLIC_HEADER "${TZ_HEADERS}"
            VERSION "${PROJECT_VERSION}"
            SOVERSION "${ABI_VERSION}")
    if (NOT MSVC)
        find_package(Threads)
        target_link_libraries(date-tz PUBLIC Threads::Threads)
    endif ()
    if (NOT USE_SYSTEM_TZ_DB AND NOT MANUAL_TZ_DB)
        find_package(CURL REQUIRED)
        target_include_directories(date-tz SYSTEM PRIVATE ${CURL_INCLUDE_DIRS})
        target_link_libraries(date-tz PRIVATE ${CURL_LIBRARIES})
    endif ()
endif ()
