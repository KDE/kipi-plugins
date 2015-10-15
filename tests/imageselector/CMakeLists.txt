#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(imageselector_SRCS actionthread.cpp
                       main.cpp
                       imageselector.cpp
)

add_executable(imageselector ${imageselector_SRCS})

target_link_libraries(imageselector
                      PRIVATE
                      Qt5::Gui
                      Qt5::Core

                      KF5::Kipi
                      KF5::KExiv2

                      KF5kipiplugins
)
