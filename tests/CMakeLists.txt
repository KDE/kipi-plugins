#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

project(kipitest)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(${KEXIV2_DEFINITIONS})

set(kipicmd_sources
    kipiinterface.cpp
    kipiimagecollectionshared.cpp
    kipiimageinfoshared.cpp
    kipiimagecollectionselector.cpp
    kipiuploadwidget.cpp
    kipitest-debug.cpp
    main.cpp
   )

KDE4_ADD_EXECUTABLE(kipicmd ${kipicmd_sources})

target_link_libraries(kipicmd
                      ${KDE4_UI_LIBS}
                      ${KDE4_KPARTS_LIBS}
                      ${KEXIV2_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      kipiplugins
                     )

add_subdirectory(multithreading)
