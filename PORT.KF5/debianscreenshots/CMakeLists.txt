#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${QJSON_INCLUDE_DIR})

set(kipiplugin_debianscreenshots_PART_SRCS
    plugin_debianscreenshots.cpp
    dswindow.cpp
    dswidget.cpp
    dstalker.cpp
    packagedelegate.cpp
    debshots.h
    mpform.cpp
    kclickableimagelabel.cpp
#    dsjob.cpp
   )

qt5_add_resources(kipiplugin_debianscreenshots_QRC_SRCS plugin_debianscreenshots.qrc)

add_library(kipiplugin_debianscreenshots MODULE ${kipiplugin_debianscreenshots_PART_SRCS} ${kipiplugin_debianscreenshots_QRC_SRCS} )

target_link_libraries(kipiplugin_debianscreenshots
                      kipiplugins
                      KF5::Kipi
                      ${QJSON_LIBRARIES}
                      KF5::KExiv2
                      KF5::KDcraw
                      KF5::KIOCore
                      
                     )

configure_file(kipiplugin_debianscreenshots.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_debianscreenshots.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_debianscreenshots.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_debianscreenshots                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_debianscreenshotsui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
