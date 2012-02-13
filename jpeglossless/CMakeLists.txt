SUBDIRS(icons)

# enable final triggers: http://bugs.kde.org/show_bug.cgi?id=126326
# digikam: camera download: auto-rotated images loose EXIF info ...
# So make sure nofinal is always used here!
KDE4_NO_ENABLE_FINAL(kipiplugin_jpeglossless)

INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg
                    ${CMAKE_CURRENT_SOURCE_DIR}/transform
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                   )

SET(kipiplugin_jpeglossless_PART_SRCS plugin/plugin_jpeglossless.cpp
                                      plugin/utils.cpp
                                      plugin/actionthread.cpp
                                      transform/jpegtransform.cpp
                                      transform/imagerotate.cpp
                                      transform/convert2grayscale.cpp
                                      transform/imageflip.cpp
                                      libjpeg/transupp.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_jpeglossless ${kipiplugin_jpeglossless_PART_SRCS})

IF(WIN32)
    SET_TARGET_PROPERTIES( kipiplugin_jpeglossless PROPERTIES COMPILE_FLAGS -DJPEG_STATIC )
ENDIF(WIN32)

TARGET_LINK_LIBRARIES(kipiplugin_jpeglossless
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${JPEG_LIBRARY}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_jpeglossless DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES plugin/kipiplugin_jpeglossless.desktop DESTINATION ${SERVICES_INSTALL_DIR})
