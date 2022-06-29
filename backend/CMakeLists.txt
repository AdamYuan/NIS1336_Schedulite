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
        )
add_library(schedulite::backend ALIAS ScheduliteBackend)

find_package(Threads REQUIRED)

target_link_libraries(ScheduliteBackend
        PRIVATE schedulite::backend::dep
        PUBLIC Threads::Threads
        )
target_include_directories(ScheduliteBackend PUBLIC include)
