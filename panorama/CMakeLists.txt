ADD_SUBDIRECTORY(icons)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/importwizard
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
#                     ${CMAKE_CURRENT_SOURCE_DIR}/ptogenerator
                    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser
                    ${CMAKE_CURRENT_SOURCE_DIR}/ptotype
                    ${CMAKE_CURRENT_SOURCE_DIR}/tasks
                    ${Boost_INCLUDE_DIR}/boost              # NOTICE: this is the only way for my system to compile it
#                     ${Boost_INCLUDE_DIRS}
                   )

# Enable exceptions for boost::spirit::karma, because of a bug in
#     boost/numeric/conversion/converter_policies.hpp
# that prevent us from karma without exceptions
# Note that adding -fexceptions is not enough because -DQT_NO_EXCEPTIONS
# should be unset at the same time, hence the following line:
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

INSTALL(DIRECTORY pics
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_panorama)

# =======================================================
# Common source code

SET(panorama_common_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/ptotype/ptotype.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/controlpointline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/imageline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/optimisationline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/projectline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/ptoparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ptoparser/variousgrammar.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/ptogenerator/ptogenerator.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/createptotask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/cpfindtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/cpcleantask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/optimisationtask.cpp
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

INSTALL(TARGETS kipiplugin_panorama DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES plugin/kipiplugin_panorama.desktop DESTINATION ${SERVICES_INSTALL_DIR})

# =======================================================
# Stand alone version of Panorama plugin

SET(panorama_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/panorama.cpp)

KDE4_ADD_APP_ICON(panoramagui_SRCS "./icons/*panorama.png")

KDE4_ADD_EXECUTABLE(panoramagui ${panorama_SRCS})

TARGET_LINK_LIBRARIES(panoramagui libpanorama)

INSTALL(TARGETS panoramagui ${INSTALL_TARGETS_DEFAULT_ARGS})
INSTALL(PROGRAMS plugin/panoramagui.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
