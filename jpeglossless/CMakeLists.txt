subdirs(pics)

# enable final triggers: http://bugs.kde.org/show_bug.cgi?id=126326
# digikam: camera download: auto-rotated images loose EXIF info ...
# So make sure nofinal is always used here!
kde4_no_enable_final(kipiplugin_jpeglossless)

set(kipiplugin_jpeglossless_PART_SRCS plugin_jpeglossless.cpp transupp.cpp jpegtransform.cpp utils.cpp imagerotate.cpp convert2grayscale.cpp imageflip.cpp actionthread.cpp)

kde4_automoc(${kipiplugin_jpeglossless_PART_SRCS})

kde4_add_plugin(kipiplugin_jpeglossless ${kipiplugin_jpeglossless_PART_SRCS})

target_link_libraries(kipiplugin_jpeglossless ${KDE4_KPARTS_LIBS} ${QT_AND_KDECORE_LIBS} ${KIPI_LIBRARIES} ${KEXIV2_LIBRARIES} ${QT_QT3SUPPORT_LIBRARY} ${JPEG_LIBRARY})

install(TARGETS kipiplugin_jpeglossless DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kipiplugin_jpeglossless.desktop DESTINATION ${SERVICES_INSTALL_DIR})
