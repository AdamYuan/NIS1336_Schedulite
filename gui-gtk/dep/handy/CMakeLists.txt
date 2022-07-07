cmake_minimum_required(VERSION 3.15)
project(Schedulite)

find_package(PkgConfig REQUIRED)
pkg_check_modules(HANDY REQUIRED libhandy-1)

add_library(handy_interface INTERFACE)
add_library(handy::interface ALIAS handy_interface)

target_compile_options(handy_interface INTERFACE ${HANDY_CFLAGS_OTHER})
target_include_directories(handy_interface INTERFACE ${HANDY_INCLUDE_DIRS})
target_link_directories(handy_interface INTERFACE ${HANDY_LIBRARY_DIRS})
target_link_libraries(handy_interface INTERFACE ${HANDY_LIBRARIES})