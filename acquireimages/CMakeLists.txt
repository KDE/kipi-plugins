SET(kipiplugin_acquireimages_PART_SRCS plugin_acquireimages.cpp scandialog.cpp)

INCLUDE_DIRECTORIES(${SANE_INCLUDE_DIR})

KDE4_ADD_PLUGIN(kipiplugin_acquireimages ${kipiplugin_acquireimages_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_acquireimages 
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KSANE_LIBRARIES}
                      ${KEXIV2_LIBRARIES} 
                      ${QT_QT3SUPPORT_LIBRARY}
                      kipiplugins
                     )

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

INSTALL(TARGETS kipiplugin_acquireimages DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_acquireimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
