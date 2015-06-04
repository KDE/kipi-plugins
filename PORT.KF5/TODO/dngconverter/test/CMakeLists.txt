#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# =======================================================
# DNGVALIDATE tool from DNG SDK

set(dngvalidate_SRCS ../dngwriter/extra/dng_sdk/dng_validate.cpp)

add_executable(dngvalidate ${dngvalidate_SRCS})
ecm_mark_nongui_executable(dngvalidate)

target_link_libraries(dngvalidate libdng ${CMAKE_THREAD_LIBS_INIT})

# =======================================================
# DNGINFO command line tool

set(dnginfo_SRCS dnginfo.cpp)

add_executable(dnginfo ${dnginfo_SRCS})
ecm_mark_nongui_executable(dnginfo)

target_link_libraries(dnginfo
                      libdng
                      KF5::KDELibs4Support

                      ${QT_LIBRARIES}
                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                     )

# =======================================================
# RAW2DNG command line tool

set(raw2dng_SRCS raw2dng.cpp)

add_executable(raw2dng
ecm_mark_nongui_executable(raw2dng)
                    ${libdngwriter_SRCS}
                    ${raw2dng_SRCS}
                   )

target_link_libraries(raw2dng
                      KF5::KDELibs4Support
                      KF5::KIOCore
 
                      ${QT_LIBRARIES}
                      KF5::KExiv2
                      KF5::KDcraw
                      ${CMAKE_THREAD_LIBS_INIT}
                      libdng
                      KF5kipiplugins
                     )
