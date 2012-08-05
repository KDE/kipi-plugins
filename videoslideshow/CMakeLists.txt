ADD_SUBDIRECTORY(magickiface)

INCLUDE_DIRECTORIES(${ImageMagick_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
                    ${CMAKE_CURRENT_SOURCE_DIR}/magickiface
                   )

SET(kipiplugin_videoslideshow_PART_SRCS plugin/plugin_videoslideshow.cpp
                                        manager/actionthread.cpp
                                        dialogs/exportdialog.cpp
                                        dialogs/myimagelist.cpp
                                        dialogs/slideshowsettingswidget.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_videoslideshow ${kipiplugin_videoslideshow_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_videoslideshow
                      magickiface
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${ImageMagick_LIBRARIES}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_videoslideshow DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES plugin/kipiplugin_videoslideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})

INSTALL(FILES plugin/kipiplugin_videoslideshowui.rc DESTINATION ${DATA_INSTALL_DIR}/kipi)
