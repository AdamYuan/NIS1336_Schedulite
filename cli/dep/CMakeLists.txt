cmake_minimum_required(VERSION 3.15)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(tabulate)

add_library(ScheduliteCLI_dep INTERFACE)
add_library(schedulite::cli::dep ALIAS ScheduliteCLI_dep)

target_include_directories(ScheduliteCLI_dep INTERFACE cxxopts)
target_link_libraries(ScheduliteCLI_dep INTERFACE tabulate::tabulate)
