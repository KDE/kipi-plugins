INCLUDE_DIRECTORIES(${KSANE_INCLUDE_DIR})

# =======================================================
# Common source code

SET(acquireimages_common_SRCS
    scandialog.cpp
    saveimgthread.cpp
   )

KDE4_ADD_LIBRARY(libacquireimages STATIC ${acquireimages_common_SRCS})

TARGET_LINK_LIBRARIES(libacquireimages
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KSANE_LIBRARY}
                      ${KEXIV2_LIBRARIES}
                      kipiplugins
                     )

# =======================================================
# Plugin

SET(kipiplugin_acquireimages_PART_SRCS plugin_acquireimages.cpp)

KDE4_ADD_PLUGIN(kipiplugin_acquireimages ${kipiplugin_acquireimages_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_acquireimages libacquireimages)

INSTALL(TARGETS kipiplugin_acquireimages DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_acquireimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})

# =======================================================
# Stand alone version

SET(scangui_SRCS scangui.cpp)

KDE4_ADD_APP_ICON(scangui_SRCS "scanner")
KDE4_ADD_EXECUTABLE(scangui ${scangui_SRCS})

TARGET_LINK_LIBRARIES(scangui libacquireimages)

INSTALL(TARGETS scangui ${INSTALL_TARGETS_DEFAULT_ARGS})
INSTALL(PROGRAMS scangui.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
