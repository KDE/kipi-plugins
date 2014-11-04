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

KDE4_ADD_UI_FILES(kipiplugin_htmlexport_SRCS
                  imagesettingspage.ui
                  outputpage.ui
                  themepage.ui
                  themeparameterspage.ui
                 )

KDE4_ADD_KCFG_FILES(kipiplugin_htmlexport_SRCS
                    htmlexportconfig.kcfgc
                   )

KDE4_ADD_PLUGIN(kipiplugin_htmlexport ${kipiplugin_htmlexport_SRCS})

target_link_libraries(kipiplugin_htmlexport
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${LIBXSLT_EXSLT_LIBRARIES}
                      ${LIBXSLT_LIBRARIES}
                      ${LIBXML2_LIBRARIES}
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_QTCORE_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      kipiplugins
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
install(FILES     kipiplugin_htmlexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
install(FILES     THEME_HOWTO                                               DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_htmlexport)
install(DIRECTORY themes                                                    DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_htmlexport)
