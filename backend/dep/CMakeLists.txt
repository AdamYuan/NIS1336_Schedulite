cmake_minimum_required(VERSION 3.15)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(PlatformFolders)
add_subdirectory(cpp-ipc)
add_subdirectory(filesystem)
add_subdirectory(stduuid)
add_subdirectory(nowide)

add_library(ScheduliteBackend_dep INTERFACE)
add_library(schedulite::backend::dep ALIAS ScheduliteBackend_dep)

add_library(ScheduliteBackend_dep_nowide INTERFACE)
add_library(schedulite::backend::dep::nowide ALIAS ScheduliteBackend_dep_nowide)
target_link_libraries(ScheduliteBackend_dep_nowide INTERFACE nowide::nowide)

target_include_directories(ScheduliteBackend_dep INTERFACE picosha2 plusaes)
target_link_libraries(ScheduliteBackend_dep INTERFACE sago::platform_folders ipc ghc_filesystem stduuid ScheduliteBackend_dep_nowide)
