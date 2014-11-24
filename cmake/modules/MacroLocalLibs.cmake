# Some useful macros to detect local libs
#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro(DETECT_LIBKIPI MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKIPI)

        message(STATUS "libkipi : search system based library")
        find_package(KF5Kipi ${MIN_VERSION})
        set(KF5Kipi_LIBRARIES KF5::Kipi)

    else()

        message(STATUS "libkipi : use local library from ${CMAKE_SOURCE_DIR}/extra/libkipi/")
        find_file(KF5Kipi_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkipi/)

        if(NOT KF5Kipi_FOUND)
            message(ERROR "libkipi : local library not found")
        endif()

        set(KF5Kipi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extra/libkipi/src ${CMAKE_BINARY_DIR}/extra/libkipi/src ${CMAKE_BINARY_DIR}/extra/libkipi)
        set(KF5Kipi_LIBRARIES KF5Kipi)

    endif()

    message(STATUS "libkipi include dir : ${KF5Kipi_INCLUDE_DIRS}")
    message(STATUS "libkipi library     : ${KF5Kipi_LIBRARIES}")

endmacro()

macro(DETECT_LIBKDCRAW MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKDCRAW)

        message(STATUS "libkdcraw : search system based library")
        find_package(KF5KDcraw ${MIN_VERSION})
        set(KF5KDcraw_LIBRARIES KF5::KDcraw)

    else()

        message(STATUS "libkdcraw : use local library from ${CMAKE_SOURCE_DIR}/extra/libkdcraw/")
        find_file(KF5KDcraw_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkdcraw/)

        if(NOT KF5KDcraw_FOUND)
            message(ERROR "libkdcraw : local library not found")
        endif()

        set(KF5KDcraw_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extra/libkdcraw/src ${CMAKE_BINARY_DIR}/extra/libkdcraw/src ${CMAKE_BINARY_DIR}/extra/libkdcraw)
        set(KF5KDcraw_LIBRARIES KF5KDcraw)

    endif()

    message(STATUS "libkdcraw include dir : ${KF5KDcraw_INCLUDE_DIRS}")
    message(STATUS "libkdcraw library     : ${KF5KDcraw_LIBRARIES}")

endmacro()

macro(DETECT_LIBKEXIV2 MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKEXIV2)

        message(STATUS "libkexiv2 : search system based library")
        find_package(KF5KExiv2 ${MIN_VERSION})
        set(KF5KExiv2_LIBRARIES KF5::KExiv2)

    else()

        message(STATUS "libkexiv2 : use local library from ${CMAKE_SOURCE_DIR}/extra/libkexiv2/")
        find_file(KF5KExiv2_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkexiv2/)

        if(NOT KF5KExiv2_FOUND)
            message(ERROR "libkexiv2 : local library not found")
        endif()

        set(KF5KExiv2_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extra/libkexiv2/src ${CMAKE_BINARY_DIR}/extra/libkexiv2/src ${CMAKE_BINARY_DIR}/extra/libkexiv2)
        set(KF5KExiv2_LIBRARIES KF5KExiv2)

    endif()

    message(STATUS "libkexiv2 include dir : ${KF5KExiv2_INCLUDE_DIRS}")
    message(STATUS "libkexiv2 library     : ${KF5KExiv2_LIBRARIES}")

endmacro()
