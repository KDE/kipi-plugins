#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_flickr\")

set(kipiplugin_flickr_PART_SRCS
    plugin_flickr.cpp
    flickrwindow.cpp
    login.cpp
    flickrtalker.cpp
    flickrwidget.cpp
    flickrlist.cpp
    comboboxdelegate.cpp
    comboboxintermediate.cpp
    mpform.cpp
    selectuserdlg.cpp
    newalbum.cpp
   )

add_library(kipiplugin_flickr MODULE ${kipiplugin_flickr_PART_SRCS})

target_link_libraries(kipiplugin_flickr
                      PRIVATE
                      KF5kipiplugins
                      KF5::Kipi
                      KF5::I18n
                      KF5::KIOCore
                      KF5::KIOWidgets
                      KF5::WindowSystem
                     )

configure_file(kipiplugin_flickr.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_flickr.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_flickr.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_flickr                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_flickrui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
