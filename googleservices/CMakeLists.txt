#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_googleservices\")

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

add_library(kipiplugin_googleservices MODULE ${kipiplugin_googleservices_PART_SRCS})

target_link_libraries(kipiplugin_googleservices
    PRIVATE
        KF5::Kipi
        KF5::KExiv2
        KF5::KIOCore
        KF5::KIOWidgets
        KF5::WindowSystem
        KF5::IconThemes
        KF5kipiplugins
)

configure_file(kipiplugin_googleservices.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_googleservices.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_googleservices.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_googleservices                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_googleservicesui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
