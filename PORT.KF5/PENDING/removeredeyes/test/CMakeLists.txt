#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(NOT WIN32)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../plugin
                        ${CMAKE_CURRENT_SOURCE_DIR}/../libcvblobs
                        ${OpenCV_INCLUDE_DIR}
                       )

    set(testipp_SRCS testForIPP.cpp)

    KDE4_ADD_EXECUTABLE(testipp NOGUI
                        ${testipp_SRCS}
                       )

    target_link_libraries(testipp
                          ${OpenCV_LIBRARIES}
                         )

endif()
