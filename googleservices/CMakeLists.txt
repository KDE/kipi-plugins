#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${QJSON_INCLUDE_DIR})

set(kipiplugin_googleservices_PART_SRCS
    authorize.cpp
    replacedialog.cpp
    mpform_gdrive.cpp
    mpform_picasa.cpp
    plugin_googleservices.cpp
    gswidget.cpp
    gswindow.cpp
    newalbumdlg.cpp
    gdtalker.cpp
    picasawebtalker.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_googleservices ${kipiplugin_googleservices_PART_SRCS})

target_link_libraries(kipiplugin_googleservices
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QJSON_LIBRARIES}
                      kipiplugins
                     )

configure_file(kipiplugin_googleservices.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_googleservices.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_googleservices.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_googleservices                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_googleservicesui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
