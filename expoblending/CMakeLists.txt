#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/importwizard
                    ${CMAKE_CURRENT_SOURCE_DIR}/blendingdlg
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                   )

install(DIRECTORY pics
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_expoblending
       )

# =======================================================
# Common source code

set(expoblending_common_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/actionthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/enfusebinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/importwizarddlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/intropage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/itemspage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/preprocessingpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/lastpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/blendingdlg/expoblendingdlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/blendingdlg/enfusesettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/blendingdlg/bracketstack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/blendingdlg/enfusestack.cpp
   )

# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)
foreach(_currentfile ${expoblending_common_SRCS})
    if(NOT MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
endforeach()

KDE4_ADD_LIBRARY(libexpoblending STATIC ${expoblending_common_SRCS})

target_link_libraries(libexpoblending
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

# =======================================================
# ExpoBlending plugin

set(kipiplugin_expoblending_PART_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin_expoblending.cpp)

KDE4_ADD_PLUGIN(kipiplugin_expoblending ${kipiplugin_expoblending_PART_SRCS})

target_link_libraries(kipiplugin_expoblending libexpoblending)

configure_file(plugin/kipiplugin_expoblending.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_expoblending.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_expoblending.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_expoblending                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_expoblendingui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)

# =======================================================
# Stand alone version of ExpoBlending plugin

set(expoblending_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/expoblending.cpp)

KDE4_ADD_APP_ICON(expoblending_SRCS "./icons/*kipi-expoblending.png")

KDE4_ADD_EXECUTABLE(expoblending ${expoblending_SRCS})

target_link_libraries(expoblending libexpoblending)

install(TARGETS expoblending ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS plugin/expoblending.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
