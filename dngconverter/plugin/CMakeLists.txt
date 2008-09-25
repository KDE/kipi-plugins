# Disable DNG SDK warnings: we will never touch this code.
FOREACH(_curentfile ${libdng_SRCS})
    SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS -w)
ENDFOREACH(_curentfile ${libdng_SRCS})

# Disable XMP SDK warnings: we will never touch this code.
FOREACH(_curentfile ${libxmp_SRCS})
    SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS -w)
ENDFOREACH(_curentfile ${libxmp_SRCS})

SET(kipiplugin_dngconverter_PART_SRCS plugin_dngconverter.cpp
                                      actionthread.cpp
                                      batchdialog.cpp
                                      settingswidget.cpp
                                      ${libdngwriter_SRCS}
   )

KDE4_ADD_PLUGIN(kipiplugin_dngconverter ${kipiplugin_dngconverter_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_dngconverter 
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${EXPAT_LIBRARY}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_dngconverter DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_dngconverter.desktop DESTINATION ${SERVICES_INSTALL_DIR})
