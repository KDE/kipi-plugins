
SET(kipiplugin_advancedslideshow_PART_SRCS commoncontainer.cpp
                                           screenproperties.cpp
                                           captiondialog.cpp
                                           soundtrackdialog.cpp
                                           advanceddialog.cpp
                                           playbackwidget.cpp
                                           slideplaybackwidget.cpp
                                           slideshowconfig.cpp
                                           slideshowloader.cpp
                                           listsounditems.cpp
                                           slideshow.cpp
                                           kbeffect.cpp
                                           imageloadthread.cpp
                                           plugin_advancedslideshow.cpp
                                           slideshowkb.cpp
                                           slideshowgl.cpp
                                           maindialog.cpp
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

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

INSTALL(FILES KIPIicon.svg DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_advancedslideshow)
INSTALL(TARGETS kipiplugin_advancedslideshow DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_advancedslideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})
