SET(kipiplugin_htmlexport_SRCS
    abstractthemeparameter.cpp
    colorthemeparameter.cpp
    galleryinfo.cpp
    generator.cpp
    imagegenerationfunctor.cpp
    intthemeparameter.cpp
    invisiblebuttongroup.cpp
    listthemeparameter.cpp
    plugin.cpp
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

TARGET_LINK_LIBRARIES(kipiplugin_htmlexport
                      ${KIPI_LIBRARIES}
                      ${LIBEXSLT_LIBRARIES}
                      ${LIBXSLT_LIBRARIES}
                      ${LIBXML2_LIBRARIES}
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_QTCORE_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      kipiplugins
                     )

INCLUDE_DIRECTORIES(${LIBXSLT_INCLUDE_DIR}
                    ${LIBXML2_INCLUDE_DIR}
                   )

ADD_DEFINITIONS(${LIBXSLT_DEFINITIONS}
                ${LIBXML2_DEFINITIONS}
               )

INSTALL(TARGETS kipiplugin_htmlexport
        DESTINATION ${PLUGIN_INSTALL_DIR}
       )

INSTALL(FILES kipiplugin_htmlexport.desktop
        DESTINATION ${SERVICES_INSTALL_DIR}
       )

INSTALL(FILES THEME_HOWTO
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_htmlexport
       )

INSTALL(DIRECTORY themes
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_htmlexport
        PATTERN ".svn" EXCLUDE
       )
