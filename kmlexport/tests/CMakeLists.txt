#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_executable(test_kmlexport_gpxparsing
                    test_kmlexport_gpxparsing.cpp
                   )

target_link_libraries(test_kmlexport_gpxparsing
                      KF5::KDELibs4Support
                      Qt5::Test
                      Qt5::Gui
                     )

add_test(test_kmlexport_gpxparsing ${EXECUTABLE_OUTPUT_PATH}/test_kmlexport_gpxparsing)

