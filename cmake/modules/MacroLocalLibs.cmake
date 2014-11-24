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
        set(KF5Kipi_LIBRARIES KF5::Kipi PARENT_SCOPE)

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
