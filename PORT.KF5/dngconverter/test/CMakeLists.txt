#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# =======================================================
# DNGVALIDATE tool from DNG SDK

set(dngvalidate_SRCS ../dngwriter/extra/dng_sdk/dng_validate.cpp)

KDE4_ADD_EXECUTABLE(dngvalidate NOGUI ${dngvalidate_SRCS})

target_link_libraries(dngvalidate libdng ${CMAKE_THREAD_LIBS_INIT})

# =======================================================
# RAW2DNG command line tool

set(raw2dng_SRCS raw2dng.cpp)

KDE4_ADD_EXECUTABLE(raw2dng NOGUI
                    ${libdngwriter_SRCS}
                    ${raw2dng_SRCS}
                   )

target_link_libraries(raw2dng
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${QT_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      libdng
                      kipiplugins
                     )
