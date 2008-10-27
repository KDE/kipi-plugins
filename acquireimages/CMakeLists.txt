IF(WIN32)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/twainiface)

    SET(twainiface_SRCS
        twainiface/dib.cpp
        twainiface/dibfile.c
        twainiface/dibutil.c
        twainiface/main.cpp
        twainiface/qtwain.cpp
        twainiface/qtwaininterface.cpp
        twainiface/qtwainmainwindow.cpp
        twainiface/qtwainsubstitute.cpp
        twainiface/twaincpp.cpp
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
ELSEIF(WIN32)

    SET(kipiplugin_acquireimages_PART_SRCS plugin_acquireimages.cpp scandialog.cpp)

    INCLUDE_DIRECTORIES(${KSANE_INCLUDE_DIR})

    KDE4_ADD_PLUGIN(kipiplugin_acquireimages ${kipiplugin_acquireimages_PART_SRCS})

    TARGET_LINK_LIBRARIES(kipiplugin_acquireimages 
                          ${KDE4_KPARTS_LIBS} 
                          ${QT_AND_KDECORE_LIBS} 
                          ${KIPI_LIBRARIES} 
                          ${KSANE_LIBRARY}
                          ${KEXIV2_LIBRARIES} 
                          kipiplugins
                        )

    KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

    INSTALL(TARGETS kipiplugin_acquireimages DESTINATION ${PLUGIN_INSTALL_DIR})

    INSTALL(FILES kipiplugin_acquireimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})

ENDIF(WIN32)
