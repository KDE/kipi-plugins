ADD_DEFINITIONS(-DQT3_SUPPORT 
                -DQT3_SUPPORT_WARNINGS
               )

SET(kipiplugin_slideshow_PART_SRCS common.cpp
                                   kbeffect.cpp 
                                   screenproperties.cpp
                                   slideshowkb.cpp 
                                   imageloadthread.cpp
                                   listimageitems.cpp
				   imageviewer.cpp
				   maindialog.cpp
				   captiondialog.cpp
				   soundtrackdialog.cpp
				   advanceddialog.cpp 
				   playbackwidget.cpp
				   slideplaybackwidget.cpp
                                   slideshow.cpp 
                                   slideshowconfig.cpp 
                                   slideshowgl.cpp 
                                   slideshowloader.cpp
				   listsounditems.cpp
                                   plugin_slideshow.cpp 
   )

KDE4_ADD_UI_FILES(kipiplugin_slideshow_PART_SRCS maindialog.ui 
						 captiondialog.ui 
						 advanceddialog.ui
						 soundtrackdialog.ui
						 slideplaybackwidget.ui
						 playbackwidget.ui
		 )

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
                      ${KDE4_KDE3SUPPORT_LIBS}
		      ${KDE4_PHONON_LIBRARY}
                      ${X11_Xrandr_LIB}
                      kipiplugins
                     )

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})
install(FILES KIPIicon.svg DESTINATION ${DATA_INSTALL_DIR}//kipiplugin_slideshow/)

INSTALL(TARGETS kipiplugin_slideshow DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_slideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})
