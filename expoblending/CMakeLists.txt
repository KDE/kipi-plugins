SUBDIRS(icons)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/importwizard
                    ${CMAKE_CURRENT_SOURCE_DIR}/blendingdlg
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                   )

SET(expoblending_common_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/actionthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/alignbinary.cpp
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

IF(OPENMP_FOUND AND NOT APPLE)
    ADD_DEFINITIONS(${OpenMP_CXX_FLAGS})
    IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL GNU )
        SET(OPENMP_LDFLAGS "-lgomp")
    ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL GNU )
    IF("${CMAKE_C_COMPILER_ID}" STREQUAL SunPro )
        SET(OPENMP_LDFLAGS "-xopenmp")
    ENDIF("${CMAKE_C_COMPILER_ID}" STREQUAL SunPro )
ENDIF(OPENMP_FOUND AND NOT APPLE)

INSTALL(DIRECTORY pics
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_expoblending
       )

# =======================================================
# ExpoBlending plugin

SET(kipiplugin_expoblending_PART_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin_expoblending.cpp

    ${expoblending_common_SRCS}
   )

KDE4_ADD_PLUGIN(kipiplugin_expoblending ${kipiplugin_expoblending_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_expoblending
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${OPENMP_LDFLAGS}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_expoblending DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES plugin/kipiplugin_expoblending.desktop DESTINATION ${SERVICES_INSTALL_DIR})

# =======================================================
# Stand alone version of ExpoBlending plugin

SET(expoblending_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin/expoblending.cpp
    ${expoblending_common_SRCS}
   )

KDE4_ADD_APP_ICON(hdrgui_SRCS "expoblending")

KDE4_ADD_EXECUTABLE(expoblending ${expoblending_SRCS})

TARGET_LINK_LIBRARIES(expoblending
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${OPENMP_LDFLAGS}
                      kipiplugins
                     )

INSTALL(TARGETS expoblending ${INSTALL_TARGETS_DEFAULT_ARGS})
INSTALL(PROGRAMS plugin/expoblending.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
