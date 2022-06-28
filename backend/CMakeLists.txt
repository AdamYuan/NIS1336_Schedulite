cmake_minimum_required(VERSION 3.15)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(dep/private)
add_subdirectory(dep/interface)

add_library(ScheduliteBackend SHARED
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
        PRIVATE schedulite::backend::dep::private
        PUBLIC schedulite::backend::dep::interface Threads::Threads
        )
target_include_directories(ScheduliteBackend PUBLIC include)
