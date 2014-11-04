#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(THREADWEAVER_FOUND ${KDE4_THREADWEAVER_LIBRARIES})

if(THREADWEAVER_FOUND)

    include_directories(${KDE4_INCLUDES}
                        ${JPEG_INCLUDE_DIR}
                        ${QT_INCLUDE_DIR}
                        ${QT_QTCORE_INCLUDE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../jpeglossless/plugin
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../jpeglossless/transform
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../jpeglossless/libjpeg
                       )

    set(multithread_SRCS
        main.cpp
        imageselector.cpp
        ../../jpeglossless/transform/convert2grayscale.cpp
        ../../jpeglossless/transform/jpegtransform.cpp
        ../../jpeglossless/transform/imagerotate.cpp
        ../../jpeglossless/transform/imageflip.cpp
        ../../jpeglossless/libjpeg/transupp.cpp
        ../../jpeglossless/plugin/utils.cpp
        ../../jpeglossless/plugin/actionthread.cpp
    )

    KDE4_ADD_EXECUTABLE(multithread ${multithread_SRCS})

    target_link_libraries(multithread
                          ${QT_QTGUI_LIBRARY}
                          ${QT_AND_KDECORE_LIBS}
                          ${KDE4_KPARTS_LIBS}
                          ${JPEG_LIBRARY}
                          ${KEXIV2_LIBRARIES}
                          ${KDCRAW_LIBRARIES}
                          ${KDE4_THREADWEAVER_LIBRARIES}
                          ${PNG_LIBRARIES}
                          kipiplugins
                         )

endif()
