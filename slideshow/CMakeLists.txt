ADD_DEFINITIONS(-DQT3_SUPPORT 
                -DQT3_SUPPORT_WARNINGS
               )
SET(kipiplugin_slideshow_PART_SRCS toolbar.cpp                                   
                                   kbeffect.cpp 
                                   screenproperties.cpp
                                   slideshowkb.cpp 
                                   imageloadthread.cpp
                                   #listimageitems.cpp 
                                   #slideshow.cpp 
                                   #slideshowconfig.cpp 
                                   #slideshowgl.cpp 
                                   #slideshowloader.cpp
                                   #plugin_slideshow.cpp 
   )
#kde4_add_ui3_files(kipiplugin_slideshow_PART_SRCS slideshowconfigbase.ui)

KDE4_ADD_PLUGIN(kipiplugin_slideshow ${kipiplugin_slideshow_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_slideshow 
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

INSTALL(TARGETS kipiplugin_slideshow DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_slideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})
