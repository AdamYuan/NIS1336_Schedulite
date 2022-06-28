cmake_minimum_required(VERSION 3.13)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(stduuid)

add_library(ScheduliteBackend_dep_interface INTERFACE)
add_library(schedulite::backend::dep::interface ALIAS ScheduliteBackend_dep_interface)

target_link_libraries(ScheduliteBackend_dep_interface INTERFACE stduuid)
