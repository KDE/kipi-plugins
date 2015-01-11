#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

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

install(TARGETS dnginfo ${INSTALL_TARGETS_DEFAULT_ARGS})
