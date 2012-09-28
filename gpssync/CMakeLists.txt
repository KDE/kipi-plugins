SUBDIRS(icons)

INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/../")
INCLUDE_DIRECTORIES(${KGEOMAP_INCLUDE_DIR})

SET(kipiplugin_gpssync_PART_SRCS
    gpssyncdialog.cpp
    plugin_gpssync.cpp
    kipiimagemodel.cpp
    kipiimageitem.cpp
    mapdragdrophandler.cpp
    kipiimagelist.cpp
    gpsimagelistdragdrophandler.cpp
    gpslistviewcontextmenu.cpp
    gpscorrelatorwidget.cpp
    gpsimagedetails.cpp
    gpsundocommand.cpp
    gpsreversegeocodingwidget.cpp
    gpsbookmarkowner.cpp
    backend-rg.cpp
    backend-geonames-rg.cpp
    backend-osm-rg.cpp
    searchbackend.cpp
    searchwidget.cpp
    setup.cpp
    setup_general.cpp
    backend-geonamesUS-rg.cpp
    rgtagmodel.cpp
    track_correlator.cpp
    track_correlator_thread.cpp
    track_listmodel.cpp
    tests/simpletreemodel/simpletreemodel.cpp
   )

IF(KDE4_BUILD_TESTS)
    INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/borrowed")
    #ADD_DEFINITIONS(-DGPSSYNC_MODELTEST)
    SET(kipiplugin_gpssync_PART_SRCS
        ${kipiplugin_gpssync_PART_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/borrowed/modeltest.cpp
       )
ENDIF(KDE4_BUILD_TESTS)

IF (NOT ${KDE_VERSION} VERSION_GREATER "4.4.81")
    # these are borrowed from a new KDE trunk version
    # and are needed for sorting of the view
    INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/borrowed-future")
    SET(borrowed_future_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/borrowed-future/klinkitemselectionmodel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/borrowed-future/kmodelindexproxymapper.cpp
       )
    SET(kipiplugin_gpssync_PART_SRCS
        ${kipiplugin_gpssync_PART_SRCS}
        ${borrowed_future_SRCS}
       )
ENDIF (NOT ${KDE_VERSION} VERSION_GREATER "4.4.81")

KDE4_ADD_PLUGIN(kipiplugin_gpssync ${kipiplugin_gpssync_PART_SRCS})

# TODO: verify that the libkgeomap library is installed!
TARGET_LINK_LIBRARIES(kipiplugin_gpssync
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_KHTML_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KGEOMAP_LIBRARIES}
                      kipiplugins
                     )

IF(KDE4_BUILD_TESTS)
    # needed by modeltest
    TARGET_LINK_LIBRARIES(kipiplugin_gpssync
                          ${QT_QTTEST_LIBRARY}
                          kipiplugins
                         )
ENDIF(KDE4_BUILD_TESTS)

CONFIGURE_FILE(kipiplugin_gpssync.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_gpssync.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_gpssync.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_gpssync                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_gpssyncui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)

INSTALL(FILES
    data/bookmarks-marker.png
    data/searchmarker-normal.png
    data/searchmarker-selected.png
    DESTINATION ${DATA_INSTALL_DIR}/gpssync/)

IF(KDE4_BUILD_TESTS)
    MACRO_OPTIONAL_ADD_SUBDIRECTORY(tests)
ENDIF(KDE4_BUILD_TESTS)
