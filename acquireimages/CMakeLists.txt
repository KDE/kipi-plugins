INCLUDE_DIRECTORIES(${KSANE_INCLUDE_DIR})

SET(kipiplugin_acquireimages_PART_SRCS
    plugin_acquireimages.cpp
    scandialog.cpp
    saveimgthread.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_acquireimages ${kipiplugin_acquireimages_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_acquireimages
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KSANE_LIBRARY}
                      ${KEXIV2_LIBRARIES}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_acquireimages DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_acquireimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})

# =======================================================
# Stand alone version of acquireimages plugin

SET(scangui_SRCS scangui.cpp
                 scandialog.cpp
                 saveimgthread.cpp
   )

KDE4_ADD_APP_ICON(scangui_SRCS "scanner")

KDE4_ADD_EXECUTABLE(scangui ${scangui_SRCS})

TARGET_LINK_LIBRARIES(scangui
                      ${KDE4_KIO_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KSANE_LIBRARY}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

INSTALL(TARGETS scangui ${INSTALL_TARGETS_DEFAULT_ARGS})
INSTALL(FILES scangui.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
