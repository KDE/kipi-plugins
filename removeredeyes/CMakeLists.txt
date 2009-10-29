
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/libcvblobs
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/savemethods
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/locators
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/locators/haarclassifier
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/locators/haarclassifier/widgets
                    ${CMAKE_CURRENT_SOURCE_DIR}/commonwidgets
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

SET(locators_haarclassifier_SRCS
    detection/locators/haarclassifier/haarclassifierlocator.cpp
    detection/locators/haarclassifier/widgets/advancedsettings.cpp
    detection/locators/haarclassifier/widgets/blobsettingsbox.cpp
    detection/locators/haarclassifier/widgets/simplesettings.cpp
    detection/locators/haarclassifier/widgets/classifiersettingsbox.cpp
    detection/locators/haarclassifier/widgets/haarsettingswidget.cpp
   )

SET(locators_SRCS
    detection/locators/locatorfactory.cpp
   )

SET(savemethods_SRCS
    detection/savemethods/savemethods.cpp
    detection/savemethods/savemethodfactory.cpp
   )

SET(commonwidgets_SRCS
    commonwidgets/controlwidget.cpp
    commonwidgets/infomessagewidget.cpp
    commonwidgets/myimageslist.cpp
    commonwidgets/previewwidget.cpp
    commonwidgets/storagesettingsbox.cpp
    commonwidgets/unprocessedsettingsbox.cpp
   )
   
SET(kipiplugin_removeredeyes_PART_SRCS
    plugin_removeredeyes.cpp
    removeredeyeswindow.cpp
    workerthread.cpp
    workerthreaddata.cpp
    ${savemethods_SRCS}
    ${locators_SRCS}
    ${locators_haarclassifier_SRCS}
    ${libcvblobs_SRCS}
    ${commonwidgets_SRCS}
   )

KDE4_ADD_PLUGIN(kipiplugin_removeredeyes ${kipiplugin_removeredeyes_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_removeredeyes
                      ${KIPI_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KEXIV2_LIBRARIES} 
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

INSTALL(FILES commonwidgets/controlwidget.svg removeredeyes_classifier_eye_20_20.xml
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_removeredeyes)

INSTALL(FILES kipiplugin_removeredeyes.desktop DESTINATION ${SERVICES_INSTALL_DIR})
