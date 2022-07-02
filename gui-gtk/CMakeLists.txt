cmake_minimum_required(VERSION 3.16)
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

install(TARGETS ScheduliteGTK RUNTIME DESTINATION gtk-gui)
if (MINGW)
    install(FILES "${CMAKE_MINGW_BIN}/gdbus.exe" DESTINATION gtk-gui) # install gdbus.exe
    # install other dll dependencies
    install(CODE "set (CMAKE_MINGW_BIN \"${CMAKE_MINGW_BIN}\")")
    install(CODE [[
    file(GET_RUNTIME_DEPENDENCIES
        RESOLVED_DEPENDENCIES_VAR RES_DEPS
        UNRESOLVED_DEPENDENCIES_VAR UNRES_DEPS
        CONFLICTING_DEPENDENCIES_PREFIX CONFLICTING_DEPENDENCIES
        EXECUTABLES $<TARGET_FILE:ScheduliteGTK>
        DIRECTORIES ${CMAKE_MINGW_BIN}
        PRE_EXCLUDE_REGEXES "api-ms-*" "ext-ms-*"
        POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
    )

    message("\n\nFound dependencies:")
    foreach(DEP ${RES_DEPS})
        message("${DEP}")
        file(INSTALL
            DESTINATION "${CMAKE_INSTALL_PREFIX}/gtk-gui"
            TYPE SHARED_LIBRARY
            FOLLOW_SYMLINK_CHAIN
            FILES "${DEP}"
        )
    endforeach()

    message("\n\nNot found dependencies:")
    foreach(DEP ${UNRES_DEPS})
        message("${DEP}")
    endforeach()
    ]])
endif ()
