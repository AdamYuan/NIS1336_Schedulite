cmake_minimum_required(VERSION 3.15)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

if (NOT TARGET schedulite::backend)
    add_subdirectory(../backend)
endif ()
add_subdirectory(dep)

add_executable(ScheduliteGTK
        src/main.cpp)

target_link_libraries(ScheduliteGTK PRIVATE schedulite::gui::dep schedulite::backend)
target_include_directories(ScheduliteGTK PRIVATE include)

install(TARGETS ScheduliteGTK RUNTIME DESTINATION)
if (MINGW)
    set(CMAKE_GET_RUNTIME_DEPENDENCIES_TOOL objdump)
    install(CODE [[
    file(GET_RUNTIME_DEPENDENCIES
        RESOLVED_DEPENDENCIES_VAR RES
        UNRESOLVED_DEPENDENCIES_VAR UNRES
        CONFLICTING_DEPENDENCIES_PREFIX CONFLICTING_DEPENDENCIES
        EXECUTABLES ScheduliteGTK
    )
    message("\n\nFound dependencies :")
    foreach(DEP ${RES})
        message("${DEP}")
    endforeach()
    message("\n\nNot found dependencies :")
    foreach(DEP ${UNRES})
        message("${DEP}")
    endforeach()
    ]])
endif ()
