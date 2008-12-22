
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/libcvblobs
                    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
                    ${OpenCV_CV_INCLUDE_DIR}
                    ${OpenCV_CXCORE_INCLUDE_DIR}
                    ${OpenCV_CVAUX_INCLUDE_DIR}
                    ${OpenCV_HIGHGUI_INCLUDE_DIR}
                   )

SET(libcvblobs_SRCS
    libcvblobs/Blob.cpp
    libcvblobs/BlobResult.cpp
    libcvblobs/BlobExtraction.cpp
   )

SET(widgets_SRCS
    widgets/advancedsettings.cpp
    widgets/blobsettingsbox.cpp
    widgets/controlwidget.cpp
    widgets/classifiersettingsbox.cpp
    widgets/infomessagewidget.cpp
    widgets/myimageslist.cpp
    widgets/previewwidget.cpp
    widgets/settingstab.cpp
    widgets/simplesettings.cpp
    widgets/storagesettingsbox.cpp
    widgets/unprocessedsettingsbox.cpp
   )
   
SET(kipiplugin_removeredeyes_PART_SRCS
    plugin_removeredeyes.cpp
    removeredeyeswindow.cpp
    workerthread.cpp
    workerthreaddata.cpp
    eyelocator.cpp
    ${libcvblobs_SRCS}
    ${widgets_SRCS}
   )

KDE4_ADD_PLUGIN(kipiplugin_removeredeyes ${kipiplugin_removeredeyes_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_removeredeyes
                      ${KIPI_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${QT_AND_KDECORE_LIBS}
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${OpenCV_CV_LIBRARY}
                      ${OpenCV_CXCORE_LIBRARY}
                      ${OpenCV_CVAUX_LIBRARY}
                      ${OpenCV_HIGHGUI_LIBRARY}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_removeredeyes DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES widgets/controlwidget.svg removeredeyes_classifier_eye_20_20.xml 
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_removeredeyes)
INSTALL(FILES kipiplugin_removeredeyes.desktop DESTINATION ${SERVICES_INSTALL_DIR})
