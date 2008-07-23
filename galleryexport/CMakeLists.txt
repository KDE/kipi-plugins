
SET( kipiplugin_galleryexport_PART_SRCS
    plugin_galleryexport.cpp
    galleries.cpp
    gallerylist.cpp
    galleryconfig.cpp
    gallerywindow.cpp
    gallerytalker.cpp
    gallerywidget.cpp
    gallerympform.cpp
    galleryviewitem.cpp
)

KDE4_ADD_UI_FILES( kipiplugin_galleryexport_PART_SRCS galleryalbumdialog.ui )

# run adjam, remove it!!
#ADD_DEFINITIONS(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

KDE4_ADD_PLUGIN( kipiplugin_galleryexport ${kipiplugin_galleryexport_PART_SRCS} )

TARGET_LINK_LIBRARIES ( kipiplugin_galleryexport 
                        #kwalletclient 
                        ${KDE4_KDE3SUPPORT_LIBS}
                        ${KDE4_KHTML_LIBS} 
                        ${KDE4_KIO_LIBS} 
                        ${KDE4_KDEUI_LIBS} 
                        kipiplugins
)

KDE4_INSTALL_ICONS( ${ICON_INSTALL_DIR} )

INSTALL(TARGETS kipiplugin_galleryexport DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_galleryexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})

INSTALL(FILES  gallery.png DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_galleryexport/pics)
