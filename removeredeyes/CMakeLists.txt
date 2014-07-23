ADD_SUBDIRECTORY(test)

ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/libcvblobs
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/savemethods
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/locators
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/locators/haarclassifier
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/locators/haarclassifier/widgets
                    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${OpenCV_INCLUDE_DIR}
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

SET(widgets_SRCS
    widgets/controlwidget.cpp
    widgets/infomessagewidget.cpp
    widgets/myimageslist.cpp
    widgets/previewwidget.cpp
    widgets/storagesettingsbox.cpp
    widgets/unprocessedsettingsbox.cpp
   )

SET(kipiplugin_removeredeyes_PART_SRCS
    plugin/plugin_removeredeyes.cpp
    plugin/removeredeyeswindow.cpp
    plugin/workerthread.cpp
    plugin/workerthreaddata.cpp
    ${savemethods_SRCS}
    ${widgets_SRCS}
    ${locators_SRCS}
    ${locators_haarclassifier_SRCS}
    ${libcvblobs_SRCS}
   )

KDE4_ADD_PLUGIN(kipiplugin_removeredeyes ${kipiplugin_removeredeyes_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_removeredeyes
                      ${KIPI_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${QT_AND_KDECORE_LIBS}
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${OpenCV_LIBRARIES}
                      kipiplugins
                     )

CONFIGURE_FILE(plugin/kipiplugin_removeredeyes.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_removeredeyes.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_removeredeyes.desktop     DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_removeredeyes                                                DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   plugin/kipiplugin_removeredeyesui.rc                                    DESTINATION ${DATA_INSTALL_DIR}/kipi)
INSTALL(FILES   widgets/controlwidget.svg plugin/removeredeyes_classifier_eye_20_20.xml DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_removeredeyes)
