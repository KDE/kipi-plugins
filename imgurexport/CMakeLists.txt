SUBDIRS(icons)

find_library(LINUX_KQOAUTH kqoauth /usr/lib)

SET(kipiplugin_imgurexport_PART_SRCS
    plugin_imgurexport.cpp
    imgurtalker.cpp
    imgurtalkerauth.cpp
    imgurwidget.cpp
    imgurwindow.cpp
    imgurimageslist.cpp
    mpform.cpp
)

IF(LINUX_KQOAUTH)
    SET( LIB_KQOAUTH ${LINUX_KQOAUTH} )

    INCLUDE_DIRECTORIES( ${LINUX_INCLUDE_KQOAUTH} )
    ADD_DEFINITIONS(
                    -DOAUTH_ENABLED
                    ${QCA2_DEFINITIONS}
    )

    SET(kipiplugin_imgurexport_PART_SRCS
        ${kipiplugin_imgurexport_PART_SRCS}
        imgurtalkerauth.cpp
    )
ENDIF(LINUX_KQOAUTH)

KDE4_ADD_PLUGIN(kipiplugin_imgurexport ${kipiplugin_imgurexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_imgurexport
                      ${KIPI_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KEXIV2_LIBRARIES}
                      ${QJSON_LIBRARIES}
                      kipiplugins
)


IF(LINUX_KQOAUTH)
    find_file(LINUX_INCLUDE_KQOAUTH QtKOAuth /usr/include)

    TARGET_LINK_LIBRARIES(kipiplugin_imgurexport
                          ${QCA2_LIBRARIES}
                          ${LINUX_KQOAUTH}
    )

ENDIF(LINUX_KQOAUTH)

CONFIGURE_FILE(kipiplugin_imgurexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imgurexport.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imgurexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_imgurexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_imgurexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
