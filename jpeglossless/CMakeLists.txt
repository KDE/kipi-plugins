SUBDIRS(pics)

# enable final triggers: http://bugs.kde.org/show_bug.cgi?id=126326
# digikam: camera download: auto-rotated images loose EXIF info ...
# So make sure nofinal is always used here!
KDE4_NO_ENABLE_FINAL(kipiplugin_jpeglossless)

INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})

SET(kipiplugin_jpeglossless_PART_SRCS plugin_jpeglossless.cpp 
                                      transupp.cpp 
                                      jpegtransform.cpp 
                                      utils.cpp 
                                      imagerotate.cpp 
                                      convert2grayscale.cpp 
                                      imageflip.cpp 
                                      actionthread.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_jpeglossless ${kipiplugin_jpeglossless_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_jpeglossless 
                      ${KDE4_KPARTS_LIBS} 
                      ${KDE4_KDE3SUPPORT_LIBS}
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      ${KDCRAW_LIBRARIES}
                      ${JPEG_LIBRARY}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_jpeglossless DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_jpeglossless.desktop DESTINATION ${SERVICES_INSTALL_DIR})
