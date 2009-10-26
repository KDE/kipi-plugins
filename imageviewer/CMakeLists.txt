SUBDIRS(icons cursors)

SET(kipiplugin_imageviewer_PART_SRCS 
    texture.cpp
    timer.cpp
    plugin_viewer.cpp
    viewerwidget.cpp
   )

KDE4_ADD_UI_FILES(kipiplugin_imageviewer_PART_SRCS helpdialog.ui)

KDE4_ADD_PLUGIN(kipiplugin_imageviewer ${kipiplugin_imageviewer_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_imageviewer 
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${QT_QTOPENGL_LIBRARY}
                      ${OPENGL_LIBRARIES} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      ${KDCRAW_LIBRARIES} 

                      # For Linux Only.
                      ${X11_LIBRARIES}
                      ${X11_Xrandr_LIB}

                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_imageviewer DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_imageviewer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
