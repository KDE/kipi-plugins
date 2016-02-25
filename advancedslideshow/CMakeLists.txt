#
# Copyright (c) 2010-2016, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

if(Phonon4Qt5_FOUND)
    add_definitions(-DHAVE_PHONON)
endif()

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_advancedslideshow\")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dialogs
                    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${PHONON_INCLUDE_DIR}
)

set(kipiplugin_advancedslideshow_PART_SRCS dialogs/captiondialog.cpp
                                           dialogs/advanceddialog.cpp
                                           dialogs/maindialog.cpp
                                           dialogs/slideshowconfig.cpp

                                           widgets/slideplaybackwidget.cpp
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

if(Phonon4Qt5_FOUND)
    set(kipiplugin_advancedslideshow_PART_SRCS ${kipiplugin_advancedslideshow_PART_SRCS}
        widgets/listsounditems.cpp
        dialogs/soundtrackdialog.cpp
        widgets/playbackwidget.cpp
    )

    ki18n_wrap_ui(kipiplugin_advancedslideshow_PART_SRCS
                  ui/soundtrackdialog.ui
                  ui/playbackwidget.ui
    )
endif()

ki18n_wrap_ui(kipiplugin_advancedslideshow_PART_SRCS
              ui/maindialog.ui
              ui/captiondialog.ui
              ui/advanceddialog.ui
              ui/slideplaybackwidget.ui
)

add_library(kipiplugin_advancedslideshow MODULE ${kipiplugin_advancedslideshow_PART_SRCS})

target_link_libraries(kipiplugin_advancedslideshow
                      PRIVATE

                      Qt5::OpenGL
                      Qt5::Svg

                      KF5::Kipi
                      KF5::I18n
                      KF5::KIOFileWidgets
                      KF5kipiplugins

                      ${OPENGL_LIBRARIES}

                      # For Linux only.
#                      ${X11_LIBRARIES}
#                      ${X11_Xrandr_LIB}
)

if(Phonon4Qt5_FOUND)
    target_link_libraries(kipiplugin_advancedslideshow
                          PRIVATE
                          ${PHONON_LIBRARIES}
    )
endif()

configure_file(plugin/kipiplugin_advancedslideshow.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_advancedslideshow.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_advancedslideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_advancedslideshow                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_advancedslideshowui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
