cmake_minimum_required(VERSION 3.15)
project(TinyFileDialogs)

set(CMAKE_CXX_STANDARD 17)

add_library(tinyfiledialogs STATIC tinyfiledialogs.c)
target_include_directories(tinyfiledialogs PUBLIC .)
