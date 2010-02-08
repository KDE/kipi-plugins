SUBDIRS(icons)

SET(kipiplugin_imgurexport_PART_SRCS
   plugin_imgurexport.cpp
   imguruploader.cpp
   webservice.cpp
)

KDE4_ADD_PLUGIN(kipiplugin_imgurexport ${kipiplugin_imgurexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_imgurexport
                      ${KIPI_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_imgurexport DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_imgurexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
