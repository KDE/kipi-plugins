SUBDIRS(icons)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/importwizard
                    ${CMAKE_CURRENT_SOURCE_DIR}/blendingdlg
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                   )

INSTALL(DIRECTORY pics
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_expoblending
       )

# =======================================================
# Common source code

SET(expoblending_common_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/actionthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/enfusebinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/importwizarddlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/intropage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/itemspage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/preprocessingpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/lastpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/blendingdlg/expoblendingdlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/blendingdlg/enfusesettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/blendingdlg/bracketstack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/blendingdlg/enfusestack.cpp
   )

# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (B.K.O: #269903)
FOREACH(_currentfile ${expoblending_common_SRCS})
    IF(NOT MSVC)
        SET_SOURCE_FILES_PROPERTIES(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    ENDIF(NOT MSVC)
ENDFOREACH(_currentfile ${expoblending_common_SRCS})

KDE4_ADD_LIBRARY(libexpoblending STATIC ${expoblending_common_SRCS})

TARGET_LINK_LIBRARIES(libexpoblending
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

# =======================================================
# ExpoBlending plugin

SET(kipiplugin_expoblending_PART_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin_expoblending.cpp)

KDE4_ADD_PLUGIN(kipiplugin_expoblending ${kipiplugin_expoblending_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_expoblending libexpoblending)

CONFIGURE_FILE(plugin/kipiplugin_expoblending.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_expoblending.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_expoblending.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_expoblending                                            DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   plugin/kipiplugin_expoblendingui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)

# =======================================================
# Stand alone version of ExpoBlending plugin

SET(expoblending_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/expoblending.cpp)

KDE4_ADD_APP_ICON(expoblending_SRCS "./icons/*kipi-expoblending.png")

KDE4_ADD_EXECUTABLE(expoblending ${expoblending_SRCS})

TARGET_LINK_LIBRARIES(expoblending libexpoblending)

INSTALL(TARGETS expoblending ${INSTALL_TARGETS_DEFAULT_ARGS})
INSTALL(PROGRAMS plugin/expoblending.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
