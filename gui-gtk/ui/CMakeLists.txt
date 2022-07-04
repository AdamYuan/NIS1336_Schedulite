cmake_minimum_required(VERSION 3.15)
project(Schedulite)

set(UI_SRC
        User
        )

function(add_ui_header UI_FILE HEADER_FILE)
    add_custom_command(OUTPUT ${HEADER_FILE}
            COMMAND ${CMAKE_COMMAND} -E echo 'constexpr const char *k${UI_NAME}UIString = R\"\(' > ${HEADER_FILE}
            COMMAND ${CMAKE_COMMAND} -E cat ${UI_FILE} >> ${HEADER_FILE}
            COMMAND ${CMAKE_COMMAND} -E echo '\)\"\;' >> ${HEADER_FILE}
            DEPENDS ${UI_FILE})
endfunction()

set(UI_DEP)
foreach (UI_NAME IN LISTS UI_SRC)
    set(UI_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${UI_NAME}.glade)
    set(HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/ui/${UI_NAME}.hpp)
    add_ui_header(${UI_FILE} ${HEADER_FILE})
    list(APPEND UI_DEP ${HEADER_FILE})
endforeach ()

add_custom_target(
        ScheduliteGTK_ui_header_gen
        DEPENDS ${UI_DEP}
)
add_library(ScheduliteGTK_ui INTERFACE)
add_library(schedulite::gtk::ui ALIAS ScheduliteGTK_ui)
add_dependencies(ScheduliteGTK_ui ScheduliteGTK_ui_header_gen)
target_include_directories(ScheduliteGTK_ui INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/include)
