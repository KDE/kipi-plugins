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

add_executable(testmagickiface ${testmagickiface_SRCS})

target_link_libraries(testmagickiface
                      magickiface
                      Qt5::Test
                      Qt5::Gui
                      KF5::KDELibs4Support
                      ${ImageMagick_LIBRARIES}
                     )

# interactive test, not usable as test suite
# add_test(NAME testmagickiface COMMAND testmagickiface)
