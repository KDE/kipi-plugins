#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(kipiplugin_htmlexport_SRCS
    abstractthemeparameter.cpp
    colorthemeparameter.cpp
    galleryinfo.cpp
    generator.cpp
    imagegenerationfunctor.cpp
    intthemeparameter.cpp
    invisiblebuttongroup.cpp
    listthemeparameter.cpp
    plugin_htmlexport.cpp
    stringthemeparameter.cpp
    theme.cpp
    wizard.cpp
   )

ki18n_wrap_ui(kipiplugin_htmlexport_SRCS
                  imagesettingspage.ui
                  outputpage.ui
                  themepage.ui
                  themeparameterspage.ui
                 )

kconfig_add_kcfg_files(kipiplugin_htmlexport_SRCS
                    htmlexportconfig.kcfgc
                   )

add_library(kipiplugin_htmlexport MODULE ${kipiplugin_htmlexport_SRCS})

target_link_libraries(kipiplugin_htmlexport
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KDcraw
                      ${LIBXSLT_EXSLT_LIBRARIES}
                      ${LIBXSLT_LIBRARIES}
                      ${LIBXML2_LIBRARIES}
                      KF5::KDELibs4Support
                      KF5::KIOCore
                      Qt5::Core
                      Qt5::Gui
                      KF5kipiplugins
                     )

include_directories(${LIBXSLT_INCLUDE_DIR}
                    ${LIBXML2_INCLUDE_DIR}
                   )

add_definitions(${LIBXSLT_DEFINITIONS}
                ${LIBXML2_DEFINITIONS}
               )

configure_file(kipiplugin_htmlexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_htmlexport.desktop)

install(FILES     ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_htmlexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS   kipiplugin_htmlexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES     kipiplugin_htmlexportui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
install(FILES     THEME_HOWTO                                               DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_htmlexport)
install(DIRECTORY themes                                                    DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_htmlexport)
