#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_advancedslideshow\")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dialogs
                    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${PHONON_INCLUDE_DIR}

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

                                           common/kpsvgpixmaprenderer.cpp
   )

ki18n_wrap_ui(kipiplugin_advancedslideshow_PART_SRCS
              ui/maindialog.ui
              ui/captiondialog.ui
              ui/advanceddialog.ui
              ui/soundtrackdialog.ui
              ui/slideplaybackwidget.ui
              ui/playbackwidget.ui
             )

add_library(kipiplugin_advancedslideshow MODULE ${kipiplugin_advancedslideshow_PART_SRCS})

target_link_libraries(kipiplugin_advancedslideshow
                      PRIVATE

                      Qt5::OpenGL
                      Qt5::Svg

                      KF5::Kipi
                      KF5::KExiv2
                      KF5::Parts
                      KF5::KIOFileWidgets
                      KF5kipiplugins

                      ${OPENGL_LIBRARIES}
                      ${PHONON_LIBRARIES}

                      # For Linux only.
                      ${X11_LIBRARIES}
                      ${X11_Xrandr_LIB}
                     )

configure_file(plugin/kipiplugin_advancedslideshow.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_advancedslideshow.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_advancedslideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_advancedslideshow                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_advancedslideshowui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
