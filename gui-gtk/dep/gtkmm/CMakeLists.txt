cmake_minimum_required(VERSION 3.15)
project(Schedulite)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

add_library(gtkmm_interface INTERFACE)
add_library(gtkmm::interface ALIAS gtkmm_interface)

target_compile_options(gtkmm_interface INTERFACE ${GTKMM_CFLAGS_OTHER})
target_include_directories(gtkmm_interface INTERFACE ${GTKMM_INCLUDE_DIRS})
target_link_directories(gtkmm_interface INTERFACE ${GTKMM_LIBRARY_DIRS})
target_link_libraries(gtkmm_interface INTERFACE ${GTKMM_LIBRARIES})