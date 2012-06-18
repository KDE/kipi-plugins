INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../hupnp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../hupnp/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../hupnp_av/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../hupnp_av/include
                   )

SET(AvTest_SRCS
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

KDE4_ADD_UI_FILES(AvTest_SRCS
                  controlpoint.ui
                  mainwindow.ui
                  mediarenderer_connectionwindow.ui
                  mediaserver_window.ui
                  invokeactiondialog.ui
                  mediamanager_dialog.ui
                  mediarenderer_window.ui
                 )

FOREACH(_currentfile ${AvTest_SRCS})
    IF(NOT MSVC)
        SET_SOURCE_FILES_PROPERTIES(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    ENDIF(NOT MSVC)
ENDFOREACH(_currentfile ${AvTest_SRCS})

KDE4_ADD_EXECUTABLE(avtest ${AvTest_SRCS})

TARGET_LINK_LIBRARIES(avtest
                    HUpnp
                    HUpnpAv
                    ${KDE4_KDEUI_LIBS}
                    ${QT_LIBRARIES}
                    ${QT_QTNETWORK_LIBRARY}
                    ${QT_PHONON_LIBRARY}
                    ${QT_QTXML_LIBRARY}
                    ${QT_QTXMLPATTERNS_LIBRARY}
                    )
