cmake_minimum_required(VERSION 3.15)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(gtkmm)

add_library(ScheduliteGTK_dep INTERFACE)
add_library(schedulite::gtk::dep ALIAS ScheduliteGTK_dep)

target_include_directories(ScheduliteGTK_dep INTERFACE)
target_link_libraries(ScheduliteGTK_dep INTERFACE gtkmm::interface)
