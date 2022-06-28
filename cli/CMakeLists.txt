cmake_minimum_required(VERSION 3.13)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

if (NOT TARGET schedulite::backend)
    add_subdirectory(../backend)
endif ()
add_subdirectory(dep)

add_executable(ScheduliteCLI src/main.cpp)

target_link_libraries(ScheduliteCLI PRIVATE schedulite::cli::dep schedulite::backend)

install(TARGETS ScheduliteCLI RUNTIME DESTINATION)
