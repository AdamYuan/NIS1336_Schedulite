cmake_minimum_required(VERSION 3.16)
project(Schedulite)

set(CMAKE_CXX_STANDARD 17)

if (NOT TARGET schedulite::backend)
    add_subdirectory(../backend)
endif ()
add_subdirectory(dep)
add_subdirectory(ui)

add_executable(ScheduliteGTK
        src/main.cpp
        src/Window.cpp
        src/UserBox.cpp
        src/TaskFlowBox.cpp
        src/TaskFlowBoxChild.cpp
        src/EnumFilterBox.cpp
        src/TimePopover.cpp
        src/TaskInsertBox.cpp
        src/TaskDetailBox.cpp
        src/EnumSelectPopover.cpp
        )

target_link_libraries(ScheduliteGTK PRIVATE schedulite::gtk::dep schedulite::gtk::ui schedulite::backend)
target_include_directories(ScheduliteGTK PRIVATE include)

install(TARGETS ScheduliteGTK RUNTIME DESTINATION bin)
if (MINGW)
    install(CODE "set (CMAKE_MINGW_BIN \"${CMAKE_MINGW_BIN}\")")
    install(CODE "set (CMAKE_MINGW_LIB \"${CMAKE_MINGW_LIB}\")")
    install(CODE "set (CMAKE_MINGW_PATH \"${CMAKE_MINGW_PATH}\")")
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
            DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
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
    install(FILES
            ${CMAKE_MINGW_PATH}/share/icons/hicolor/index.theme
            DESTINATION share/icons/hicolor)
    #install(DIRECTORY ${CMAKE_MINGW_PATH}/share/icons/Adwaita
    #        DESTINATION share/icons)
    #install(CODE "execute_process(COMMAND gtk-update-icon-cache \${CMAKE_INSTALL_PREFIX}/share/icons/Adwaita)")

    # Install Papirus icon theme
    set(ICONS
            apps/preferences-desktop-personal
            actions/document-edit
            status/starred
            status/semi-starred
            status/non-starred
            panel/user-status-pending
            emblems/emblem-urgent
            )
    set(SCALES 16 22 32 48 64 96 128 256 512)
    install(FILES ${CMAKE_MINGW_PATH}/share/icons/Adwaita/index.theme
            DESTINATION share/icons/Adwaita)
    foreach (ICON IN LISTS ICONS)
        foreach (SCALE IN LISTS SCALES)
            install(CODE "
                        if(EXISTS [=[${CMAKE_ICON_THEME_PATH}/${SCALE}x${SCALE}/${ICON}.svg]=])
                            file(INSTALL
                                    DESTINATION [=[\${CMAKE_INSTALL_PREFIX}/share/icons/Adwaita/${SCALE}x${SCALE}/${ICON}.svg]=]
                                    FILES [=[${CMAKE_ICON_THEME_PATH}/${SCALE}x${SCALE}/${ICON}.svg]=]
                                 )
                        endif()
                    ")
        endforeach ()
    endforeach ()
    install(CODE "execute_process(COMMAND gtk-update-icon-cache \${CMAKE_INSTALL_PREFIX}/share/icons/Adwaita)")

    install(DIRECTORY ${CMAKE_MINGW_LIB}/gdk-pixbuf-2.0 # install gdk pixbuf cache
            DESTINATION lib
            FILES_MATCHING
            PATTERN " *.dll"
            PATTERN "*.cache")
    install(DIRECTORY ${CMAKE_MINGW_LIB}/gtk-3.0
            DESTINATION lib
            FILES_MATCHING
            PATTERN "*.dll"
            PATTERN "*.cache")
    install(DIRECTORY ${CMAKE_MINGW_PATH}/etc/gtk-3.0
            DESTINATION etc)
    install(FILES "${CMAKE_MINGW_BIN}/gdbus.exe " DESTINATION bin) # install gdbus.exe
endif ()
