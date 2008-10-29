IF(WIN32)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/twainiface)

    SET(twainiface_SRCS
        twainiface/twainiface.cpp
        twainiface/twaindialog.cpp
        twainiface/qtwain.cpp
        twainiface/twaindialog.cpp
        twainiface/main.cpp
       )

    KDE4_ADD_EXECUTABLE(twainiface ${twainiface_SRCS})

    TARGET_LINK_LIBRARIES(twainiface
                          ${KDE4_KDEUI_LIBS}
                          ${QT_AND_KDECORE_LIBS}
                          ${QT_LIBRARIES}
                          ${KEXIV2_LIBRARIES}
                          ${KDCRAW_LIBRARIES}
                          kipiplugins
                         )

    SET(kipiplugin_acquireimages_PART_SRCS
        plugin_acquireimages.cpp
        twainiface/twainiface.cpp
        twainiface/qtwain.cpp
        )

    KDE4_ADD_PLUGIN(kipiplugin_acquireimages ${kipiplugin_acquireimages_PART_SRCS})

    TARGET_LINK_LIBRARIES(kipiplugin_acquireimages
                          ${KDE4_KPARTS_LIBS}
                          ${QT_AND_KDECORE_LIBS}
                          ${KIPI_LIBRARIES}
                          ${KEXIV2_LIBRARIES}
                          kipiplugins
                        )

ELSE(WIN32)

    INCLUDE_DIRECTORIES(${KSANE_INCLUDE_DIR})

    SET(kipiplugin_acquireimages_PART_SRCS
        plugin_acquireimages.cpp
        scandialog.cpp)

    KDE4_ADD_PLUGIN(kipiplugin_acquireimages ${kipiplugin_acquireimages_PART_SRCS})

    TARGET_LINK_LIBRARIES(kipiplugin_acquireimages
                          ${KDE4_KPARTS_LIBS}
                          ${QT_AND_KDECORE_LIBS}
                          ${KIPI_LIBRARIES}
                          ${KSANE_LIBRARY}
                          ${KEXIV2_LIBRARIES}
                          kipiplugins
                        )
ENDIF(WIN32)

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

INSTALL(TARGETS kipiplugin_acquireimages DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_acquireimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
