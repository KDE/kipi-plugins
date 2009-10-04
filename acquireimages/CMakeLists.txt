INCLUDE_DIRECTORIES(${KSANE_INCLUDE_DIR})

SET(kipiplugin_acquireimages_PART_SRCS
    plugin_acquireimages.cpp
    scandialog.cpp)

KDE4_ADD_PLUGIN(kipiplugin_acquireimages ${kipiplugin_acquireimages_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_acquireimages
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KSANE_LIBRARY}
                      ${KEXIV2_LIBRARIES}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_acquireimages DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_acquireimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
