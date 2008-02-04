SET(kipiplugin_gpssync_PART_SRCS gpslistviewitem.cpp 
#                                 gpsbabelbinary.cpp 
#                                 gpsdataparser.cpp 
#                                 gpseditdialog.cpp 
#                                 gpsmapwidget.cpp 
#                                 gpssyncdialog.cpp 
#                                 plugin_gpssync.cpp 
#                                 kmlexport.cpp 
#                                 kmlexportconfig.cpp 
#                                 kmlgpsdataparser.cpp 
   )

KDE4_ADD_PLUGIN(kipiplugin_gpssync ${kipiplugin_gpssync_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_gpssync 
                      ${KDE4_KPARTS_LIBS} 
                      ${KDE4_KHTML_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      kipiplugins
                     )

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

INSTALL(TARGETS kipiplugin_gpssync DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_gpssync.desktop DESTINATION ${SERVICES_INSTALL_DIR})
