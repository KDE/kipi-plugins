# Disable DNG SDK warnings: we will never touch this code.
FOREACH(_curentfile ${libdng_SRCS})
    SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS -w)
ENDFOREACH(_curentfile ${libdng_SRCS})

# Disable XMP SDK warnings: we will never touch this code.
FOREACH(_curentfile ${libxmp_SRCS})
    SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS -w)
ENDFOREACH(_curentfile ${libxmp_SRCS})

KDE4_ADD_PLUGIN(kipiplugin_dngconverter ${kipiplugin_dngconverter_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_dngconverter 
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_dngconverter DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_dngconverter.desktop DESTINATION ${SERVICES_INSTALL_DIR})

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

# =======================================================
# Stand alone version of dngconverter plugin

SET(dngconverter_SRCS dngconverter.cpp ${kipiplugin_dngconverter_PART_SRCS})

KDE4_ADD_APP_ICON(dngconverter_SRCS "hi*-action-dngconverter.png")

KDE4_ADD_EXECUTABLE(dngconverter ${dngconverter_SRCS})

TARGET_LINK_LIBRARIES(dngconverter
                      ${EXPAT_LIBRARY}
                      ${KDE4_KDEUI_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${QT_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      kipiplugins
                     )

INSTALL(TARGETS dngconverter ${INSTALL_TARGETS_DEFAULT_ARGS})
INSTALL(FILES dngconverter.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
