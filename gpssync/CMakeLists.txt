SUBDIRS(icons)

INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/../")

SET(kipiplugin_gpssync_PART_SRCS
    gpssyncdialog.cpp
    plugin_gpssync.cpp
    kipiimagemodel.cpp
    kipiimageitem.cpp
    mapdragdrophandler.cpp
    kipiimagelist.cpp
    gpsimagelistdragdrophandler.cpp
    gpslistviewcontextmenu.cpp
    gpssettingswidget.cpp
    gpscorrelatorwidget.cpp
    gpsdataparser.cpp
    gpsimagedetails.cpp
    gpsundocommand.cpp
    gpsreversegeocodingwidget.cpp
    inputboxnocancel.cpp
    gpsbookmarkowner.cpp
    backend-rg.cpp
    backend-geonames-rg.cpp
    backend-osm-rg.cpp
    searchbackend.cpp
    searchwidget.cpp
    setup.cpp
    setup_general.cpp
    borrowed/modeltest.cpp
    backend-geonamesUS-rg.cpp
    rgtagmodel.cpp
    tests/simpletreemodel/simpletreemodel.cpp
   )

IF(KDE4_BUILD_TESTS)
    INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/borrowed")
    ADD_DEFINITIONS(-DGPSSYNC_MODELTEST)
    SET(kipiplugin_gpssync_PART_SRCS
        ${kipiplugin_gpssync_PART_SRCS}
        borrowed/modeltest.cpp
       )
ENDIF(KDE4_BUILD_TESTS)

IF (NOT ${KDE_VERSION} VERSION_GREATER "4.4.81")
    # these are borrowed from a new KDE trunk version
    # and are needed for sorting of the view
    INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/borrowed-future")
    SET(kipiplugin_gpssync_PART_SRCS
        ${kipiplugin_gpssync_PART_SRCS}
        borrowed-future/klinkitemselectionmodel.cpp
        borrowed-future/kmodelindexproxymapper.cpp
       )
ENDIF (NOT ${KDE_VERSION} VERSION_GREATER "4.4.81")

KDE4_ADD_PLUGIN(kipiplugin_gpssync ${kipiplugin_gpssync_PART_SRCS})

# TODO: verify that the libkmap library is installed!
TARGET_LINK_LIBRARIES(kipiplugin_gpssync
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_KHTML_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KMAP_LIBRARIES}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_gpssync DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_gpssync.desktop DESTINATION ${SERVICES_INSTALL_DIR})

INSTALL(FILES
    data/reverse-js.js
    data/rg-google-maps-v3.html
    data/bookmarks-marker.png
    data/searchmarker-normal.png
    data/searchmarker-selected.png
    DESTINATION ${DATA_INSTALL_DIR}/gpssync/)

IF(KDE4_BUILD_TESTS)
    MACRO_OPTIONAL_ADD_SUBDIRECTORY(tests)
ENDIF(KDE4_BUILD_TESTS)
