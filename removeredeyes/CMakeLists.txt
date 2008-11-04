
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/libcvblobs)

SET(libcvblobs_SRCS
    libcvblobs/Blob.cpp
    libcvblobs/BlobResult.cpp
    libcvblobs/BlobExtraction.cpp
   )

SET(kipiplugin_removeredeyes_PART_SRCS
    advancedsettings.cpp
    blobsettingsbox.cpp
    classifiersettingsbox.cpp
    simplesettings.cpp
    eyelocator.cpp
    imageslist.cpp
    plugin_removeredeyes.cpp
    removeredeyeswindow.cpp
    storagesettingsbox.cpp
    workerthread.cpp
    workerthreaddata.cpp
    ${libcvblobs_SRCS}
   )

KDE4_ADD_PLUGIN(kipiplugin_removeredeyes ${kipiplugin_removeredeyes_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_removeredeyes
                      kipiplugins
                      ${KIPI_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${QT_AND_KDECORE_LIBS}
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${OpenCV_LIBRARIES}
)

INSTALL(TARGETS kipiplugin_removeredeyes DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES removeredeyes_classifier_eye_20_20.xml DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_removeredeyes)
INSTALL(FILES kipiplugin_removeredeyes.desktop DESTINATION ${SERVICES_INSTALL_DIR})
