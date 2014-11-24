#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../hupnp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../hupnp/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../hupnp_av/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../hupnp_av/include
                   )

set(AvTest_SRCS
    controlpoint_navigator.cpp
    dataitem_display.cpp
    mediamanager_dialog.cpp
    mediaserver_window.cpp
    controlpoint_navigatoritem.cpp
    main.cpp
    mediarenderer_connectionwindow.cpp
    rendererconnections.cpp
    controlpoint_window.cpp
    mainwindow.cpp
    mediarenderer_window.cpp
   )

QT4_WRAP_UI(AvTest_FORMS
            controlpoint.ui
            mainwindow.ui
            mediarenderer_connectionwindow.ui
            mediaserver_window.ui
            invokeactiondialog.ui
            mediamanager_dialog.ui
            mediarenderer_window.ui
           )

set(SRCS ${AvTest_SRCS} ${AvTest_FORMS})

foreach(_currentfile ${SRCS})
    if(NOT MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
endforeach()

KDE4_ADD_EXECUTABLE(avtest ${SRCS})

target_link_libraries(avtest
                      HUpnp
                      HUpnpAv
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_PHONON_LIBS}
                      ${QT_LIBRARIES}
                      ${QT_QTNETWORK_LIBRARY}
                      ${QT_QTXML_LIBRARY}
                      ${QT_QTXMLPATTERNS_LIBRARY}
                     )

#install(TARGETS avtest ${INSTALL_TARGETS_DEFAULT_ARGS})

set(avtest_xml_files
    descriptions/herqq_avtransport_scpd.xml
    descriptions/herqq_contentdirectory_scpd.xml
    descriptions/herqq_renderingcontrol_scpd.xml
    descriptions/herqq_connectionmanager_sink_scpd.xml
    descriptions/herqq_mediarenderer_description.xml
    descriptions/herqq_connectionmanager_source_scpd.xml
    descriptions/herqq_mediaserver_description.xml
   )

#install(FILES       ${avtest_xml_files}
#        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_dlnaexport/xml)
