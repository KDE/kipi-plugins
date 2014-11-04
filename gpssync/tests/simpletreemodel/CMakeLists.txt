#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

KDE4_ADD_EXECUTABLE(gpssync_test_simpletreemodel
                    test_simpletreemodel.cpp
                    simpletreemodel.cpp
                    ../../borrowed/modeltest.cpp
                   )

target_link_libraries(gpssync_test_simpletreemodel
                      ${KDE4_KDECORE_LIBS}
                      ${QT_QTTEST_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      kipiplugins
                     )

add_test(gpssync_test_simpletreemodel ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_simpletreemodel)
