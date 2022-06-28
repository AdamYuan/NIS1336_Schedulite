cmake_minimum_required(VERSION 3.13)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(PlatformFolders)
add_subdirectory(date)
add_subdirectory(cpp-ipc)

add_library(ScheduliteBackend_dep_private INTERFACE)
add_library(schedulite::backend::dep::private ALIAS ScheduliteBackend_dep_private)

target_include_directories(ScheduliteBackend_dep_private INTERFACE .)
target_link_libraries(ScheduliteBackend_dep_private INTERFACE sago::platform_folders date::date-tz ipc)
