cmake_minimum_required(VERSION 3.13)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(PlatformFolders)
add_subdirectory(date)
add_subdirectory(cpp-ipc)
add_subdirectory(filesystem)

add_library(ScheduliteBackend_dep INTERFACE)
add_library(schedulite::backend::dep ALIAS ScheduliteBackend_dep)

target_include_directories(ScheduliteBackend_dep INTERFACE .)
target_link_libraries(ScheduliteBackend_dep INTERFACE sago::platform_folders date::date-tz ipc ghc_filesystem)
