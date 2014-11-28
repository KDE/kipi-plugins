#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${QJSON_INCLUDE_DIR})

set(kipiplugin_googledrive_PART_SRCS
    mpform.cpp
    plugin_googledrive.cpp
    gdwidget.cpp
    gdwindow.cpp
    gdalbum.cpp
    gdtalker.cpp
   )

add_library(kipiplugin_googledrive MODULE ${kipiplugin_googledrive_PART_SRCS})

target_link_libraries(kipiplugin_googledrive
                      KF5::KDcraw
                      KF5::Kipi
                      KF5::KExiv2
                      
                      KF5::KIOCore
                      ${QJSON_LIBRARIES}
                      KF5kipiplugins
                     )

configure_file(kipiplugin_googledrive.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_googledrive.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_googledrive.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_googledrive                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_googledriveui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
