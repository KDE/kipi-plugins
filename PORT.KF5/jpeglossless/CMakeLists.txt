#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

# enable final triggers: http://bugs.kde.org/show_bug.cgi?id=126326
# digikam: camera download: auto-rotated images loose EXIF info ...
# So make sure nofinal is always used here!
KDE4_NO_ENABLE_FINAL(kipiplugin_jpeglossless)

include_directories(${JPEG_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg
                    ${CMAKE_CURRENT_SOURCE_DIR}/transform
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                   )

set(kipiplugin_jpeglossless_PART_SRCS plugin/plugin_jpeglossless.cpp
                                      plugin/utils.cpp
                                      plugin/actionthread.cpp
                                      transform/jpegtransform.cpp
                                      transform/imagerotate.cpp
                                      transform/convert2grayscale.cpp
                                      transform/imageflip.cpp
                                      libjpeg/transupp.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_jpeglossless ${kipiplugin_jpeglossless_PART_SRCS})

if(WIN32)
    set_target_properties( kipiplugin_jpeglossless PROPERTIES COMPILE_FLAGS -DJPEG_STATIC )
endif()

target_link_libraries(kipiplugin_jpeglossless
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${JPEG_LIBRARY}
                      kipiplugins
                     )

configure_file(plugin/kipiplugin_jpeglossless.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_jpeglossless.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_jpeglossless.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_jpeglossless                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_jpeglosslessui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
