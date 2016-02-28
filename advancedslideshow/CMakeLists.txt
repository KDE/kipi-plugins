#
# Copyright (c) 2010-2016, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

if(Phonon4Qt5_FOUND)
    add_definitions(-DHAVE_AUDIO)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/audio)
endif()

if(OPENGLLIBS_FOUND)
    add_definitions(-DHAVE_OPENGL)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opengl)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dialogs
                    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${CMAKE_CURRENT_SOURCE_DIR}/common
                    ${PHONON_INCLUDE_DIR}
)

set(kipiplugin_advancedslideshow_PART_SRCS dialogs/captionpage.cpp
                                           dialogs/advancedpage.cpp
                                           dialogs/mainpage.cpp
                                           dialogs/slideshowconfig.cpp

                                           widgets/slideplaybackwidget.cpp
                                           widgets/slideshow.cpp

                                           common/commoncontainer.cpp
                                           common/slideshowloader.cpp
                                           common/imageloadthread.cpp
                                           common/kpsvgpixmaprenderer.cpp

                                           plugin/plugin_advancedslideshow.cpp
)

if(OPENGLLIBS_FOUND)
    set(kipiplugin_advancedslideshow_PART_SRCS ${kipiplugin_advancedslideshow_PART_SRCS}
                                               opengl/slideshowgl.cpp
                                               opengl/slideshowkb.cpp
                                               opengl/kbeffect.cpp
    )
endif()

if(Phonon4Qt5_FOUND)
    set(kipiplugin_advancedslideshow_PART_SRCS ${kipiplugin_advancedslideshow_PART_SRCS}
                                               audio/listsounditems.cpp
                                               audio/soundtrackdialog.cpp
                                               audio/playbackwidget.cpp
    )

    ki18n_wrap_ui(kipiplugin_advancedslideshow_PART_SRCS
                  audio/soundtrackdialog.ui
                  audio/playbackwidget.ui
    )
endif()

ki18n_wrap_ui(kipiplugin_advancedslideshow_PART_SRCS
              dialogs/mainpage.ui
              dialogs/captionpage.ui
              dialogs/advancedpage.ui
              widgets/slideplaybackwidget.ui
)

add_library(kipiplugin_advancedslideshow MODULE ${kipiplugin_advancedslideshow_PART_SRCS})

target_link_libraries(kipiplugin_advancedslideshow
                      PRIVATE

                      Qt5::Svg

                      KF5::Kipi
                      KF5::I18n
                      KF5kipiplugins
)

if(OPENGLLIBS_FOUND)
    target_link_libraries(kipiplugin_advancedslideshow
                          PRIVATE

                          Qt5::OpenGL
                          ${OPENGL_LIBRARIES}
    )
endif()

if(Phonon4Qt5_FOUND)
    target_link_libraries(kipiplugin_advancedslideshow
                          PRIVATE
                          ${PHONON_LIBRARIES}
    )
endif()

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_advancedslideshow\")

configure_file(plugin/kipiplugin_advancedslideshow.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_advancedslideshow.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_advancedslideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_advancedslideshow                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_advancedslideshowui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
