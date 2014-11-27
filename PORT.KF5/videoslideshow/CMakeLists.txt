#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_subdirectory(magickiface)

add_definitions(${QTGSTREAMER_DEFINITIONS} ${KDE4_ENABLE_EXCEPTIONS})

include_directories(${ImageMagick_INCLUDE_DIRS}
                    ${QTGSTREAMER_INCLUDES}
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
                    ${CMAKE_CURRENT_SOURCE_DIR}/magickiface
                   )

set(kipiplugin_videoslideshow_PART_SRCS plugin/plugin_videoslideshow.cpp
                                        manager/actionthread.cpp
                                        manager/encoderdecoder.cpp
                                        dialogs/exportdialog.cpp
                                        dialogs/myimagelist.cpp
                                        dialogs/slideshowsettingswidget.cpp
   )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

add_library(kipiplugin_videoslideshow MODULE ${kipiplugin_videoslideshow_PART_SRCS})

target_link_libraries(kipiplugin_videoslideshow
                      magickiface
                      KF5::Parts
                      KF5::ThreadWeaver
                      
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KDcraw
                      ${ImageMagick_LIBRARIES}
                      ${QTGSTREAMER_LIBRARIES}
                      ${QTGSTREAMER_UTILS_LIBRARIES}
                      kipiplugins
                     )

configure_file(plugin/kipiplugin_videoslideshow.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_videoslideshow.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_videoslideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_videoslideshow                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_videoslideshowui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
