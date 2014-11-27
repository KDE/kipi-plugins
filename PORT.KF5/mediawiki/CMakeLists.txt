#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${MEDIAWIKI_INCLUDE_DIR})

ki18n_wrap_ui(ui_src wmwindow.ui)

set(kipiplugin_wikimedia_PART_SRCS
    plugin_wikimedia.cpp
    wmwindow.cpp
    wmwidget.cpp
    wikimediajob.cpp
    wmwidget.cpp
   )

add_library(kipiplugin_wikimedia MODULE ${kipiplugin_wikimedia_PART_SRCS} ${ui_src})

target_link_libraries(kipiplugin_wikimedia
                      kipiplugins
                      mediawiki
                      ${MEDIAWIKI_LIBRARIES}
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KDcraw
                      KF5::KIOCore
                      
                     )

configure_file(kipiplugin_wikimedia.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_wikimedia.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_wikimedia.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_wikimedia                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_wikimediaui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
