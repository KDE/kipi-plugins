
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(KDE4 REQUIRED)
find_package(OpenCV REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include(KDE4Defaults)

include(MacroLibrary)

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(kipiplugin_removeredeyes_PART_SRCS
    advancedsettings.cpp
    blobsettingsbox.cpp
    classifiersettingsbox.cpp
    simplesettings.cpp
    eyelocator.cpp
    imageslist.cpp
    plugin_removeredeyes.cpp
    redeyeswindow.cpp
    storagesettingsbox.cpp
    workerthread.cpp
    wteventdata.cpp
    )

kde4_add_plugin(kipiplugin_removeredeyes ${kipiplugin_removeredeyes_PART_SRCS})

target_link_libraries(
    kipiplugin_removeredeyes
    kipiplugins
    blob
    ${KIPI_LIBRARIES} 
    ${KDCRAW_LIBRARIES} 
    ${QT_AND_KDECORE_LIBS} 
    ${KDE4_KPARTS_LIBS} 
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${OpenCV_LIBRARIES}
)

install(TARGETS kipiplugin_removeredeyes DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES  removeredeyes_classifier_eye_20_20.xml DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_removeredeyes)
install(FILES  kipiplugin_removeredeyes.desktop DESTINATION ${SERVICES_INSTALL_DIR})
