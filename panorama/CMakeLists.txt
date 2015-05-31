#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2011-2015, Benjamin Girault, <benjamin dot girault at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_panorama\")

add_subdirectory(icons)

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
# TODO: remove the following line after migration to QT5 
set_source_files_properties(${panorama_common_SRCS} PROPERTIES COMPILE_FLAGS "-Wno-deprecated-declarations -fPIC")

add_library(libpanorama STATIC ${panorama_common_SRCS})

target_link_libraries(libpanorama
                      KF5::Parts
                      KF5::KDELibs4Support
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KDcraw
                      KF5::ConfigCore
                      KF5::ThreadWeaver
                      KF5kipiplugins
                      ${KF5KDcraw_LIBRARIES}
                      ${KF5Kipi_LIBRARIES}
                      ${KF5KExiv2_LIBRARIES}
#                       threadweaver
                     )

# =======================================================
# Panorama plugin

set(kipiplugin_panorama_PART_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin_panorama.cpp)
# TODO: remove the following line after migration to QT5 
set_source_files_properties(${kipiplugin_panorama_PART_SRCS} PROPERTIES COMPILE_FLAGS "-Wno-deprecated-declarations -fPIC")

add_library(kipiplugin_panorama MODULE ${kipiplugin_panorama_PART_SRCS})

target_link_libraries(kipiplugin_panorama libpanorama)

configure_file(plugin/kipiplugin_panorama.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_panorama.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_panorama.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_panorama                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_panoramaui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)

# =======================================================
# Test program

# if(CMAKE_BUILD_TYPE MATCHES ".*[dD]ebug.*")
if(BUILD_TESTING)
    set(pto_parser_test_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/parser_test.cpp)

    add_executable(pto_parser_test ${pto_parser_test_SRCS})

    target_link_libraries(pto_parser_test libpanorama)
endif()

# =======================================================
# Stand alone version of Panorama plugin

set(panorama_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/panorama.cpp)
# TODO: remove the following line after migration to QT5 
set_source_files_properties(${panorama_SRCS} PROPERTIES COMPILE_FLAGS "-Wno-deprecated-declarations -fPIC")

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/./icons/hi*-apps-kipi-panorama.png")
ecm_add_app_icon(panorama_SRCS ICONS ${ICONS_SRCS})

add_executable(panoramagui ${panorama_SRCS})

target_link_libraries(panoramagui libpanorama)

install(TARGETS  panoramagui                            ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS plugin/panoramagui.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
