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
    string(REPLACE "\\" "/" CMAKE_ICON_THEME_PATH "${CMAKE_ICON_THEME_PATH}")
    install(CODE "set (CMAKE_MINGW_BIN \"${CMAKE_MINGW_BIN}\")")
    install(CODE "set (CMAKE_MINGW_LIB \"${CMAKE_MINGW_LIB}\")")
    install(CODE "set (CMAKE_MINGW_PATH \"${CMAKE_MINGW_PATH}\")")

    # Install DLL dependencies
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

    # Extra libraries for loading SVGs
    file(GLOB EXTRA_LIBS
            ${CMAKE_MINGW_BIN}/liblzma-[0-9]*.dll
            ${CMAKE_MINGW_BIN}/librsvg-2-[0-9]*.dll
            ${CMAKE_MINGW_BIN}/libxml2-[0-9]*.dll)
    INSTALL(FILES ${EXTRA_LIBS} DESTINATION bin)

    # Install hicolor index.theme
    install(FILES
            ${CMAKE_MINGW_PATH}/share/icons/hicolor/index.theme
            DESTINATION share/icons/hicolor)

    # Install Papirus icons
    set(ICONS
            actions/document-new-symbolic
            apps/preferences-desktop-personal
            actions/document-edit
            actions/help-about
            apps/user-info

            status/dialog-information

            status/starred
            status/semi-starred
            status/non-starred

            panel/user-status-pending
            emblems/emblem-urgent
            actions/object-select

            actions/task-due
            apps/applications-office
            apps/accessories-dictionary
            apps/preferences-desktop-accessibility
            apps/applications-games
            )
    set(SCALES
            128x128 16x16@2x 18x18@2x 22x22@2x 24x24@2x 32x32@2x
            48x48 64x64 84x84 96x96 symbolic 16x16 18x18 22x22
            24x24 32x32 42x42 48x48@2x 64x64@2x 8x8)
    foreach (ICON IN LISTS ICONS)
        foreach (SCALE IN LISTS SCALES)
            install(CODE "
                        if(EXISTS [=[${CMAKE_ICON_THEME_PATH}/${SCALE}/${ICON}.svg]=])
                            file(READ [=[${CMAKE_ICON_THEME_PATH}/${SCALE}/${ICON}.svg]=] ICON_SVG)
                            file(WRITE [=[${CMAKE_INSTALL_PREFIX}/share/icons/Adwaita/${SCALE}/${ICON}.svg]=] \"\${ICON_SVG}\")
                            message(STATUS [=[Installed icon ${SCALE}/${ICON}.svg]=])
                        endif()
                    ")
        endforeach ()
    endforeach ()
    install(FILES "${CMAKE_ICON_THEME_PATH}/index.theme"
            DESTINATION share/icons/Adwaita)
    find_program(GTK_UPDATE_ICON_CACHE NAMES gtk-update-icon-cache-3.0 gtk-update-icon-cache REQUIRED)
    install(CODE "execute_process(COMMAND ${GTK_UPDATE_ICON_CACHE} -t -f ${CMAKE_INSTALL_PREFIX}/share/icons/Adwaita)"
            CODE "message (STATUS \"Generated icon cache in ${CMAKE_INSTALL_PREFIX}/share/icons/Adwaita\")")

    # Install gdk pixbuf cache
    install(DIRECTORY ${CMAKE_MINGW_LIB}/gdk-pixbuf-2.0
            DESTINATION lib
            FILES_MATCHING
            PATTERN "*.dll"
            PATTERN "*.cache")

    # Install gdbus.exe for Gtk signal
    install(FILES "${CMAKE_MINGW_BIN}/gdbus.exe " DESTINATION bin)

    # Necessary to run extensions on windows if it is not in the path
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        install(FILES
                ${CMAKE_MINGW_BIN}/gspawn-win64-helper.exe
                ${CMAKE_MINGW_BIN}/gspawn-win64-helper-console.exe
                DESTINATION bin)
    else ()
        install(FILES
                ${CMAKE_MINGW_BIN}/gspawn-win32-helper.exe
                ${CMAKE_MINGW_BIN}/gspawn-win32-helper-console.exe
                DESTINATION bin)
    endif ()
endif ()
