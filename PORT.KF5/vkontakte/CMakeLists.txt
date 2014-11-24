#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${LIBKVKONTAKTE_INCLUDES})

set(kipiplugin_vkontakte_PART_SRCS
    plugin_vkontakte.cpp
    vkwindow.cpp
    vkalbumdialog.cpp
    albumchooserwidget.cpp
    authinfowidget.cpp
    vkapi.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_vkontakte ${kipiplugin_vkontakte_PART_SRCS})

target_link_libraries(kipiplugin_vkontakte
                      kipiplugins
                      ${KIPI_LIBRARIES}
#                      ${KDCRAW_LIBRARIES}
#                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${LIBKVKONTAKTE_LIBRARIES}
                     )

configure_file(kipiplugin_vkontakte.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_vkontakte.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_vkontakte.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_vkontakte                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_vkontakteui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
