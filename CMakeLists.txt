cmake_minimum_required(VERSION 3.15)
project(Schedulite)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DNOMINMAX)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif ()

if (MINGW)
    message("Use MinGW")
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif ()

add_subdirectory(backend)
add_subdirectory(cli)
#add_subdirectory(frontend-gui)

install(TARGETS ScheduliteBackend ScheduliteCLI RUNTIME DESTINATION)
