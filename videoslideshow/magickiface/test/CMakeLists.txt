#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${ImageMagick_INCLUDE_DIRS}
                    ${QT_INCLUDES}
                    ${CMAKE_CURRENT_BINARY_DIR}/../
                    ${CMAKE_CURRENT_SOURCE_DIR}/../
                   )

set(testmagickiface_SRCS testmagickiface.cpp)

KDE4_ADD_EXECUTABLE(testmagickiface ${testmagickiface_SRCS})

target_link_libraries(testmagickiface
                      magickiface
                      ${QT_QTTEST_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      ${KDE4_KDECORE_LIBS}
                      ${ImageMagick_LIBRARIES}
                     )

# interactive test, not usable as test suite
# add_test(NAME testmagickiface COMMAND testmagickiface)
