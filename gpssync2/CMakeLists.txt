SUBDIRS(icons)

SET(kipiplugin_gpssync_PART_SRCS
        gpssyncdialog.cpp
        plugin_gpssync.cpp
        kipiimagemodel.cpp
        kipiimageitem.cpp
        gpsimageitem.cpp
        mapdragdrophandler.cpp
        kipiimagelist.cpp
        gpsimagelistdragdrophandler.cpp
        gpssettingswidget.cpp
        gpscorrelatorwidget.cpp
        gpsdataparser.cpp
        gpsundocommand.cpp
        gpsreversegeocodingwidget.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_gpssync2 ${kipiplugin_gpssync_PART_SRCS})

# TODO: verify that the WorldMapWidget2 library is installed!
TARGET_LINK_LIBRARIES(kipiplugin_gpssync2 
                      ${KDE4_KPARTS_LIBS} 
                      ${KDE4_KHTML_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      kipiplugins
                      worldmapwidget2
                     )

INSTALL(TARGETS kipiplugin_gpssync2 DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_gpssync2.desktop DESTINATION ${SERVICES_INSTALL_DIR})

INSTALL(FILES rg-google-maps-v3.html DESTINATION ${DATA_INSTALL_DIR}/gpssync2/)


INSTALL(FILES reverse-js.js DESTINATION ${DATA_INSTALL_DIR}/gpssync2/)

IF(KDE4_BUILD_TESTS)
    MACRO_OPTIONAL_ADD_SUBDIRECTORY(tests)
ENDIF(KDE4_BUILD_TESTS)
