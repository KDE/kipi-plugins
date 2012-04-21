# ========================================================================================
# Stand alone version

SET(dngconverter_SRCS dngconverter.cpp ${dngconverter_common_SRCS})

IF(WIN32)
    REMOVE_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})
ENDIF(WIN32)

KDE4_ADD_APP_ICON(dngconverter_SRCS "../icons/*dngconverter.png")

KDE4_ADD_EXECUTABLE(dngconverter ${dngconverter_SRCS})

TARGET_LINK_LIBRARIES(dngconverter
                      ${KDE4_KIO_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      kipiplugins
                      libdng
                     )

INSTALL(TARGETS dngconverter ${INSTALL_TARGETS_DEFAULT_ARGS})
INSTALL(PROGRAMS dngconverter.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )

# ========================================================================================
# Plugin

SET(kipiplugin_dngconverter_PART_SRCS plugin_dngconverter.cpp ${dngconverter_common_SRCS})

KDE4_ADD_PLUGIN(kipiplugin_dngconverter ${kipiplugin_dngconverter_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_dngconverter
                      ${KDE4_KPARTS_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      kipiplugins
                      libdng
                     )

INSTALL(TARGETS kipiplugin_dngconverter DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_dngconverter.desktop DESTINATION ${SERVICES_INSTALL_DIR})
