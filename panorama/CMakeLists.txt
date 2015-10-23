#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2011-2015, Benjamin Girault, <benjamin dot girault at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_panorama\")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/importwizard
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser
                    ${CMAKE_CURRENT_SOURCE_DIR}/tasks
                   )

FLEX_TARGET(PTOScanner ptoparser/panoScanner.l ${CMAKE_CURRENT_BINARY_DIR}/panoScanner.c)
BISON_TARGET(PTOParser ptoparser/panoParser.y  ${CMAKE_CURRENT_BINARY_DIR}/panoParser.c)
ADD_FLEX_BISON_DEPENDENCY(PTOScanner PTOParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

install(DIRECTORY pics
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_panorama)

# =======================================================
# Common source code

set(panorama_common_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/ptotype/ptotype.cpp
    ${FLEX_PTOScanner_OUTPUTS}
    ${BISON_PTOParser_OUTPUTS}
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/tparser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/tparserprivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/tparsergetters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/ptofile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/actionthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/cpfindbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/importwizarddlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/intropage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/itemspage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/lastpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/optimizepage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/preprocessingpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/previewpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/commandtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/preprocesstask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/createfinalptotask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/createptotask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/cpfindtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/cpcleantask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/optimisationtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/autocroptask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/createpreviewtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/createmktask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/compilemksteptask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/compilemktask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/copyfilestask.cpp
   )

# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)
foreach(_currentfile ${panorama_common_SRCS})
    if(NOT MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
endforeach()

add_library(libpanorama STATIC ${panorama_common_SRCS})

target_link_libraries(libpanorama
                      KF5::Kipi
                      KF5::ConfigCore
                      KF5::ThreadWeaver
                      KF5::I18n
                      KF5kipiplugins
                     )

# =======================================================
# Panorama plugin

set(kipiplugin_panorama_PART_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin_panorama.cpp)

add_library(kipiplugin_panorama MODULE ${kipiplugin_panorama_PART_SRCS})

target_link_libraries(kipiplugin_panorama libpanorama)

configure_file(plugin/kipiplugin_panorama.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_panorama.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_panorama.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_panorama                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_panoramaui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)

# =======================================================

add_subdirectory(tests)
add_subdirectory(icons)
