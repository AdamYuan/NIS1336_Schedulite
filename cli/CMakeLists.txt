cmake_minimum_required(VERSION 3.15)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

if (NOT TARGET schedulite::backend)
    add_subdirectory(../backend)
endif ()
add_subdirectory(dep)

add_executable(ScheduliteCLI
        src/main.cpp
        src/Util.cpp
        src/Format.cpp
        src/Shell.cpp
        )

target_link_libraries(ScheduliteCLI PRIVATE schedulite::cli::dep schedulite::backend schedulite::backend::nowide)
target_include_directories(ScheduliteCLI PRIVATE include)

install(TARGETS ScheduliteCLI RUNTIME DESTINATION)
