SUBDIRS(icons)

INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/../")

SET(kipiplugin_gpssync_PART_SRCS
        gpssyncdialog.cpp
        plugin_gpssync.cpp
        kipiimagemodel.cpp
        kipiimageitem.cpp
        gpsimageitem.cpp
        mapdragdrophandler.cpp
        kipiimagelist.cpp
        gpsimagelistdragdrophandler.cpp
        gpslistviewcontextmenu.cpp
        gpssettingswidget.cpp
        gpscorrelatorwidget.cpp
        gpsdataparser.cpp
        gpsundocommand.cpp
        gpsreversegeocodingwidget.cpp
        inputboxnocancel.cpp
        gpsbookmarkowner.cpp
        backend-rg.cpp
        backend-google-rg.cpp
        backend-geonames-rg.cpp
        backend-osm-rg.cpp
        searchbackend.cpp
        searchwidget.cpp
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

INSTALL(FILES
    reverse-js.js
    rg-google-maps-v3.html
    data/bookmarks-marker.png
    data/searchmarker-normal.png
    data/searchmarker-selected.png
    DESTINATION ${DATA_INSTALL_DIR}/gpssync2/)

IF(KDE4_BUILD_TESTS)
    MACRO_OPTIONAL_ADD_SUBDIRECTORY(tests)
ENDIF(KDE4_BUILD_TESTS)
