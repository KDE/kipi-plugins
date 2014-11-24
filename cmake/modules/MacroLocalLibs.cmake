# Some useful macros to detect local or system based libraries
#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro(DETECT_LIBKIPI MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKIPI)

        message(STATUS "libkipi : search system based library")
        find_package(KF5Kipi ${MIN_VERSION})

        if(KF5Kipi_FOUND)
            set(KF5Kipi_LIBRARIES KF5::Kipi)
        endif()

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

        if(KF5KDcraw_FOUND)
            set(KF5KDcraw_LIBRARIES KF5::KDcraw)
        endif()

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

        if(KF5KExiv2_FOUND)
            set(KF5KExiv2_LIBRARIES KF5::KExiv2)
        endif()

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

macro(DETECT_LIBKFACE MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKFACE)

        message(STATUS "libkface : search system based library")
        find_package(KF5KFace ${MIN_VERSION})

        if(KF5KFace_FOUND)
            set(KF5KFace_LIBRARIES KF5::KFace)
        endif()

    else()

        message(STATUS "libkface : use local library from ${CMAKE_SOURCE_DIR}/extra/libkface/")
        find_file(KF5KFace_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkface/)

        if(NOT KF5KFace_FOUND)
            message(ERROR "libkface : local library not found")
        endif()

        set(KF5KFace_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extra/libkface/src ${CMAKE_BINARY_DIR}/extra/libkface/src ${CMAKE_BINARY_DIR}/extra/libkface)
        set(KF5KFace_LIBRARIES KF5KFace)

    endif()

    message(STATUS "libkface include dir : ${KF5KFace_INCLUDE_DIRS}")
    message(STATUS "libkface library     : ${KF5KFace_LIBRARIES}")

endmacro()

macro(DETECT_LIBKGEOMAP MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKGEOMAP)

        message(STATUS "libkgeomap : search system based library")
        find_package(KF5KGeomap ${MIN_VERSION})

        if(KF5KGeomap_FOUND)
            set(KF5KGeomap_LIBRARIES KF5::KGeomap)
        endif()

    else()

        message(STATUS "libkgeomap : use local library from ${CMAKE_SOURCE_DIR}/extra/libkgeomap/")
        find_file(KF5KGeomap_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkgeomap/)

        if(NOT KF5KGeomap_FOUND)
            message(ERROR "libkgeomap : local library not found")
        endif()

        set(KF5KGeomap_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extra/libkgeomap/src ${CMAKE_BINARY_DIR}/extra/libkgeomap/src ${CMAKE_BINARY_DIR}/extra/libkgeomap)
        set(KF5KGeomap_LIBRARIES KF5KGeomap)

    endif()

    message(STATUS "libkgeomap include dir : ${KF5KGeomap_INCLUDE_DIRS}")
    message(STATUS "libkgeomap library     : ${KF5KGeomap_LIBRARIES}")

endmacro()
