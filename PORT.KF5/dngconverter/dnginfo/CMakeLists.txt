#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# =======================================================
# DNGINFO command line tool

set(dnginfo_SRCS dnginfo.cpp)

KDE4_ADD_EXECUTABLE(dnginfo NOGUI ${dnginfo_SRCS})

target_link_libraries(dnginfo
                      libdng
                      ${KDE4_KDECORE_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${QT_LIBRARIES}
                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                     )

install(TARGETS dnginfo ${INSTALL_TARGETS_DEFAULT_ARGS})
