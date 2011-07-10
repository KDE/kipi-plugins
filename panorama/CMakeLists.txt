#SUBDIRS(icons)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/importwizard
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
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

INSTALL(DIRECTORY pics DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_panorama)

# =======================================================
# Common source code

SET(panorama_common_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/actionthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/autooptimiserbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/cpfindbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/vigoptimizebinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/importwizarddlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/intropage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/itemspage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/preprocessingpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/optimizepage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/lastpage.cpp
   )

# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (B.K.O: #269903)
FOREACH(_currentfile ${panorama_common_SRCS})
    IF(NOT MSVC)
        SET_SOURCE_FILES_PROPERTIES(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    ENDIF(NOT MSVC)
ENDFOREACH(_currentfile ${panorama_common_SRCS})

KDE4_ADD_LIBRARY(libpanorama STATIC ${panorama_common_SRCS})

TARGET_LINK_LIBRARIES(libpanorama
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${OPENMP_LDFLAGS}
                      kipiplugins
                     )

# =======================================================
# Panorama plugin

SET(kipiplugin_panorama_PART_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin_panorama.cpp)

KDE4_ADD_PLUGIN(kipiplugin_panorama ${kipiplugin_panorama_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_panorama libpanorama)

INSTALL(TARGETS kipiplugin_panorama DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES plugin/kipiplugin_panorama.desktop DESTINATION ${SERVICES_INSTALL_DIR})

# =======================================================
# Stand alone version of ExpoBlending plugin

SET(panorama_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/panorama.cpp)

KDE4_ADD_APP_ICON(panoramagui_SRCS "panorama")

KDE4_ADD_EXECUTABLE(panoramagui ${panorama_SRCS})

TARGET_LINK_LIBRARIES(panoramagui libpanorama)

INSTALL(TARGETS panoramagui ${INSTALL_TARGETS_DEFAULT_ARGS})
INSTALL(PROGRAMS plugin/panoramagui.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
