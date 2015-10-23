#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015,      Benjamin Girault, <benjamin dot girault at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_expoblending\")

add_subdirectory(icons)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/importwizard
                    ${CMAKE_CURRENT_SOURCE_DIR}/blendingdlg
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                   )

install(DIRECTORY pics
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_expoblending
       )

set(kipiplugin_expoblending_PART_SRCS
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
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin_expoblending.cpp
)

add_library(kipiplugin_expoblending MODULE ${kipiplugin_expoblending_PART_SRCS})

target_link_libraries(kipiplugin_expoblending
                      PRIVATE
                      Qt5::Core
                      Qt5::Concurrent
                      Qt5::Widgets

                      KF5kipiplugins

                      KF5::KIOFileWidgets
                      KF5::ConfigCore
                      KF5::XmlGui
                      KF5::I18n
                      KF5::Kipi
                     )

configure_file(plugin/kipiplugin_expoblending.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_expoblending.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_expoblending.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_expoblending                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_expoblendingui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
