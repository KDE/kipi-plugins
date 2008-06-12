ADD_DEFINITIONS(-DQT3_SUPPORT 
                -DQT3_SUPPORT_WARNINGS
               )

SET(kipiplugin_imageviewer_PART_SRCS 
    viewerwidget.cpp
    texture.cpp
    timer.cpp
    plugin_viewer.cpp
   )

#KDE4_ADD_UI_FILES has to be BEFORE KDE4_ADD_PLUGIN
KDE4_ADD_UI_FILES(kipiplugin_imageviewer_PART_SRCS helpdialog.ui)

KDE4_ADD_PLUGIN(kipiplugin_imageviewer ${kipiplugin_imageviewer_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_imageviewer 
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${QT_QTOPENGL_LIBRARY}
                      ${X11_LIBRARIES}
                      ${OPENGL_LIBRARIES} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      ${KDCRAW_LIBRARIES} 
                      ${QT_QT3SUPPORT_LIBRARY} 
                      ${X11_Xrandr_LIB}
                      kipiplugins
                     )

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

INSTALL(TARGETS kipiplugin_imageviewer DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_imageviewer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(FILES cursors/hand.png
              cursors/nullImage.png
              cursors/zoom.png
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_imageviewer/pics)
