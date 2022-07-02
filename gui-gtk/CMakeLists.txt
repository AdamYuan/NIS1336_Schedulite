cmake_minimum_required(VERSION 3.22)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

if (NOT TARGET schedulite::backend)
    add_subdirectory(../backend)
endif ()
add_subdirectory(dep)

add_executable(ScheduliteGTK
        src/main.cpp)

target_link_libraries(ScheduliteGTK PRIVATE schedulite::gui::dep schedulite::backend)
target_include_directories(ScheduliteGTK PRIVATE include)

install(TARGETS ScheduliteGTK
        RUNTIME_DEPENDENCY_SET ScheduliteGTKRuntimeDep
        RUNTIME DESTINATION)
if (MINGW)
    install(RUNTIME_DEPENDENCY_SET ScheduliteGTKRuntimeDep
            PRE_EXCLUDE_REGEXES "^api-ms-.*\\.dll$|^ext-ms-.*\\.dll$|^emclient\\.dll$|^hvsifiletrust\\.dll$"
            POST_EXCLUDE_REGEXES "^.*[\\\\/]system32/.*\\.dll$"
            DIRECTORIES C:/msys64/mingw64/bin
            )
endif ()
