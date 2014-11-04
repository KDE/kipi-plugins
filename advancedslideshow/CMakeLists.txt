#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dialogs
                    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                   )

set(kipiplugin_advancedslideshow_PART_SRCS dialogs/captiondialog.cpp
                                           dialogs/soundtrackdialog.cpp
                                           dialogs/advanceddialog.cpp
                                           dialogs/maindialog.cpp
                                           dialogs/slideshowconfig.cpp
                                           widgets/playbackwidget.cpp
                                           widgets/slideplaybackwidget.cpp
                                           widgets/listsounditems.cpp
                                           widgets/slideshow.cpp
                                           widgets/slideshowgl.cpp
                                           widgets/slideshowkb.cpp
                                           plugin/commoncontainer.cpp
                                           plugin/screenproperties.cpp
                                           plugin/slideshowloader.cpp
                                           plugin/kbeffect.cpp
                                           plugin/imageloadthread.cpp
                                           plugin/plugin_advancedslideshow.cpp
   )

KDE4_ADD_UI_FILES(kipiplugin_advancedslideshow_PART_SRCS
                  ui/maindialog.ui
                  ui/captiondialog.ui
                  ui/advanceddialog.ui
                  ui/soundtrackdialog.ui
                  ui/slideplaybackwidget.ui
                  ui/playbackwidget.ui
                 )

KDE4_ADD_PLUGIN(kipiplugin_advancedslideshow ${kipiplugin_advancedslideshow_PART_SRCS})

target_link_libraries(kipiplugin_advancedslideshow
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

configure_file(plugin/kipiplugin_advancedslideshow.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_advancedslideshow.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_advancedslideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_advancedslideshow                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_advancedslideshowui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
