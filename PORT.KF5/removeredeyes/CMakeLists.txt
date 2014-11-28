#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_subdirectory(test)

kde_enable_exceptions()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libcvblobs
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/savemethods
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/locators
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/locators/haarclassifier
                    ${CMAKE_CURRENT_SOURCE_DIR}/detection/locators/haarclassifier/widgets
                    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${OpenCV_INCLUDE_DIR}
                   )

set(libcvblobs_SRCS
    libcvblobs/Blob.cpp
    libcvblobs/BlobResult.cpp
    libcvblobs/BlobExtraction.cpp
   )

set(locators_haarclassifier_SRCS
    detection/locators/haarclassifier/haarclassifierlocator.cpp
    detection/locators/haarclassifier/widgets/advancedsettings.cpp
    detection/locators/haarclassifier/widgets/blobsettingsbox.cpp
    detection/locators/haarclassifier/widgets/simplesettings.cpp
    detection/locators/haarclassifier/widgets/classifiersettingsbox.cpp
    detection/locators/haarclassifier/widgets/haarsettingswidget.cpp
   )

set(locators_SRCS
    detection/locators/locatorfactory.cpp
   )

set(savemethods_SRCS
    detection/savemethods/savemethods.cpp
    detection/savemethods/savemethodfactory.cpp
   )

set(widgets_SRCS
    widgets/controlwidget.cpp
    widgets/infomessagewidget.cpp
    widgets/myimageslist.cpp
    widgets/previewwidget.cpp
    widgets/storagesettingsbox.cpp
    widgets/unprocessedsettingsbox.cpp
   )

set(kipiplugin_removeredeyes_PART_SRCS
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

add_library(kipiplugin_removeredeyes MODULE ${kipiplugin_removeredeyes_PART_SRCS})

target_link_libraries(kipiplugin_removeredeyes
                      KF5::Kipi
                      KF5::KDcraw
                      KF5::KExiv2
                      KF5::ThreadWeaver
                      
                      KF5::Parts
                      KF5::KDELibs4Support
                      
                      ${OpenCV_LIBRARIES}
                      KF5kipiplugins
                     )

configure_file(plugin/kipiplugin_removeredeyes.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_removeredeyes.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_removeredeyes.desktop     DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_removeredeyes                                                DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_removeredeyesui.rc                                    DESTINATION ${DATA_INSTALL_DIR}/kipi)
install(FILES   widgets/controlwidget.svg plugin/removeredeyes_classifier_eye_20_20.xml DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_removeredeyes)
