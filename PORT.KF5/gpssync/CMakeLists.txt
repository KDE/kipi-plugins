#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/../")
include_directories(${KGEOMAP_INCLUDE_DIR})

set(kipiplugin_gpssync_PART_SRCS
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

if(KDE4_BUILD_TESTS)
    include_directories(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/borrowed")
    #add_definitions(-DGPSSYNC_MODELTEST)
    set(kipiplugin_gpssync_PART_SRCS
        ${kipiplugin_gpssync_PART_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/borrowed/modeltest.cpp
       )
endif()

if(NOT ${KDE_VERSION} VERSION_GREATER "4.4.81")
    # these are borrowed from a new KDE trunk version
    # and are needed for sorting of the view
    include_directories(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/borrowed-future")
    set(borrowed_future_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/borrowed-future/klinkitemselectionmodel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/borrowed-future/kmodelindexproxymapper.cpp
       )
    set(kipiplugin_gpssync_PART_SRCS
        ${kipiplugin_gpssync_PART_SRCS}
        ${borrowed_future_SRCS}
       )
endif()

KDE4_ADD_PLUGIN(kipiplugin_gpssync ${kipiplugin_gpssync_PART_SRCS})

# TODO: verify that the libkgeomap library is installed!
target_link_libraries(kipiplugin_gpssync
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_KHTML_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KGEOMAP_LIBRARIES}
                      kipiplugins
                     )

if(KDE4_BUILD_TESTS)
    # needed by modeltest
    target_link_libraries(kipiplugin_gpssync
                          ${QT_QTTEST_LIBRARY}
                          kipiplugins
                         )
endif()

configure_file(kipiplugin_gpssync.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_gpssync.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_gpssync.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_gpssync                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_gpssyncui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)

install(FILES   data/bookmarks-marker.png
                data/searchmarker-normal.png
                data/searchmarker-selected.png
        DESTINATION ${DATA_INSTALL_DIR}/gpssync/)

if(KDE4_BUILD_TESTS)
    MACRO_OPTIONAL_ADD_SUBDIRECTORY(tests)
endif()
