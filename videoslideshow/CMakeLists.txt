ADD_SUBDIRECTORY(magickiface)

ADD_DEFINITIONS(${QTGSTREAMER_DEFINITIONS} ${KDE4_ENABLE_EXCEPTIONS})

INCLUDE_DIRECTORIES(${ImageMagick_INCLUDE_DIRS}
                    ${QTGSTREAMER_INCLUDES}
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
                    ${CMAKE_CURRENT_SOURCE_DIR}/magickiface
                   )

SET(kipiplugin_videoslideshow_PART_SRCS plugin/plugin_videoslideshow.cpp
                                        manager/actionthread.cpp
                                        manager/encoderdecoder.cpp
                                        dialogs/exportdialog.cpp
                                        dialogs/myimagelist.cpp
                                        dialogs/slideshowsettingswidget.cpp
   )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

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
                      ${QTGSTREAMER_LIBRARIES}
                      ${QTGSTREAMER_UTILS_LIBRARIES}
                      kipiplugins
                     )

CONFIGURE_FILE(plugin/kipiplugin_videoslideshow.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_videoslideshow.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_videoslideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_videoslideshow                                            DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   plugin/kipiplugin_videoslideshowui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
