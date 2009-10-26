SUBDIRS(icons)

SET(kipiplugin_advancedslideshow_PART_SRCS commoncontainer.cpp
                                           screenproperties.cpp
                                           captiondialog.cpp
                                           soundtrackdialog.cpp
                                           advanceddialog.cpp
                                           maindialog.cpp
                                           playbackwidget.cpp
                                           slideplaybackwidget.cpp
                                           slideshowconfig.cpp
                                           slideshowloader.cpp
                                           listsounditems.cpp
                                           slideshow.cpp
                                           kbeffect.cpp
                                           imageloadthread.cpp
                                           slideshowgl.cpp
                                           slideshowkb.cpp
                                           plugin_advancedslideshow.cpp
   )

KDE4_ADD_UI_FILES(kipiplugin_advancedslideshow_PART_SRCS
                  maindialog.ui
                  captiondialog.ui
                  advanceddialog.ui
                  soundtrackdialog.ui
                  slideplaybackwidget.ui
                  playbackwidget.ui
                 )

KDE4_ADD_PLUGIN(kipiplugin_advancedslideshow ${kipiplugin_advancedslideshow_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_advancedslideshow
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${QT_QTOPENGL_LIBRARY}
                      ${OPENGL_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_PHONON_LIBRARY}

                      # For Linux only.
                      ${X11_LIBRARIES}
                      ${X11_Xrandr_LIB}

                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_advancedslideshow DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_advancedslideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})
