ADD_SUBDIRECTORY(icons)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/importwizard
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser
                    ${CMAKE_CURRENT_SOURCE_DIR}/tasks
                   )

FLEX_TARGET(PTOScanner ptoparser/panoScanner.l ${CMAKE_CURRENT_BINARY_DIR}/panoScanner.c)
BISON_TARGET(PTOParser ptoparser/panoParser.y ${CMAKE_CURRENT_BINARY_DIR}/panoParser.c)
ADD_FLEX_BISON_DEPENDENCY(PTOScanner PTOParser)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

INSTALL(DIRECTORY pics
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_panorama)

# =======================================================
# Common source code

SET(panorama_common_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/ptotype/ptotype.cpp
    ${FLEX_PTOScanner_OUTPUTS}
    ${BISON_PTOParser_OUTPUTS}
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/tparser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/tparserprivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/tparsergetters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/ptofile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/actionthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/cpfindbinary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/importwizarddlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/intropage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/itemspage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/lastpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/optimizepage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/preprocessingpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/importwizard/previewpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/preprocesstask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/createfinalptotask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/createptotask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/cpfindtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/cpcleantask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/optimisationtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/autocroptask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/createpreviewtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/createmktask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/compilemksteptask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/compilemktask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/copyfilestask.cpp
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
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

# =======================================================
# Panorama plugin

SET(kipiplugin_panorama_PART_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin_panorama.cpp)

KDE4_ADD_PLUGIN(kipiplugin_panorama ${kipiplugin_panorama_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_panorama libpanorama)

CONFIGURE_FILE(plugin/kipiplugin_panorama.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_panorama.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_panorama.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_panorama                                            DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   plugin/kipiplugin_panoramaui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)

# =======================================================
# Stand alone version of Panorama plugin

SET(panorama_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/panorama.cpp)

KDE4_ADD_APP_ICON(panorama_SRCS "./icons/*kipi-panorama.png")

KDE4_ADD_EXECUTABLE(panoramagui ${panorama_SRCS})

TARGET_LINK_LIBRARIES(panoramagui libpanorama)

INSTALL(TARGETS  panoramagui                            ${INSTALL_TARGETS_DEFAULT_ARGS})
INSTALL(PROGRAMS plugin/panoramagui.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
