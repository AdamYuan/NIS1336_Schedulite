cmake_minimum_required(VERSION 3.13)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(dep)

add_library(ScheduliteBackend STATIC
        src/Environment.cpp
        src/User.cpp
        src/Instance.cpp
        src/Time.cpp
        src/Schedule.cpp
        src/Encryption.cpp
        src/Task.cpp
        )
add_library(schedulite::backend ALIAS ScheduliteBackend)

find_package(Threads REQUIRED)

target_link_libraries(ScheduliteBackend
        PRIVATE schedulite::backend::dep
        PUBLIC Threads::Threads
        )
target_include_directories(ScheduliteBackend PUBLIC include)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    message("Doxygen build started")

    add_custom_target(ScheduliteBackendDoxygen
            ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            DEPENDS ${DOXYGEN_IN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
endif ()
