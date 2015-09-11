# Some useful macros to detect local or system based libraries
#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro(DETECT_LIBKIPI MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKIPI)

        message(STATUS "libkipi : search system based library")
        find_package(KF5Kipi ${MIN_VERSION} CONFIG REQUIRED)

        if(KF5Kipi_FOUND)
            set(KF5Kipi_LIBRARIES KF5::Kipi)
            get_target_property(KF5Kipi_INCLUDE_DIRS KF5::Kipi INTERFACE_INCLUDE_DIRECTORIES)
            set(KF5Kipi_FOUND TRUE)
        else()
            set(KF5Kipi_FOUND FALSE)
        endif()

    else()

        message(STATUS "libkipi : use local library from ${CMAKE_SOURCE_DIR}/extra/libkipi/")
        find_file(KF5Kipi_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkipi/)

        if(NOT KF5Kipi_FOUND)
            message(ERROR "libkipi : local library not found")
            set(KF5Kipi_FOUND FALSE)
        else()
            set(KF5Kipi_FOUND TRUE)            
        endif()

        set(KF5Kipi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extra/libkipi/src ${CMAKE_BINARY_DIR}/extra/libkipi/src ${CMAKE_BINARY_DIR}/extra/libkipi)
        set(KF5Kipi_LIBRARIES KF5Kipi)

    endif()

    message(STATUS "libkipi found         : ${KF5Kipi_FOUND}")
    message(STATUS "libkipi library       : ${KF5Kipi_LIBRARIES}")
    message(STATUS "libkipi includes      : ${KF5Kipi_INCLUDE_DIRS}")

    # Detect libkipi so version used to compile kipi tool to identify if plugin can be loaded in memory by libkipi.
    # This will be used to populate plugin desktop files.

    find_file(KF5KipiConfig_FOUND libkipi_config.h PATHS ${KF5Kipi_INCLUDE_DIRS})
    file(READ "${KF5KipiConfig_FOUND}" KIPI_CONFIG_H_CONTENT)

    string(REGEX REPLACE
           ".*static +const +int +kipi_binary_version += ([^ ;]+).*"
           "\\1"
           KIPI_LIB_SO_CUR_VERSION_FOUND
           "${KIPI_CONFIG_H_CONTENT}"
          )

    set(KIPI_LIB_SO_CUR_VERSION ${KIPI_LIB_SO_CUR_VERSION_FOUND} CACHE STRING "libkipi so version")
    message(STATUS "libkipi SO version  : ${KIPI_LIB_SO_CUR_VERSION}")

endmacro()

###########################################################################################################################################"

macro(DETECT_LIBKDCRAW MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKDCRAW)

        message(STATUS "libkdcraw : search system based library")
        find_package(KF5KDcraw ${MIN_VERSION})

        if(KF5KDcraw_FOUND)
            set(KF5KDcraw_LIBRARIES KF5::KDcraw)
            get_target_property(KF5KDcraw_INCLUDE_DIRS KF5::KDcraw INTERFACE_INCLUDE_DIRECTORIES)
            set(KF5KDcraw_FOUND TRUE)
        else()
            set(KF5KDcraw_FOUND FALSE)
        endif()

    else()

        message(STATUS "libkdcraw : use local library from ${CMAKE_SOURCE_DIR}/extra/libkdcraw/")
        find_file(KF5KDcraw_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkdcraw/)

        if(NOT KF5KDcraw_FOUND)
            message(ERROR "libkdcraw : local library not found")
            set(KF5KDcraw_FOUND FALSE)
        else()
            set(KF5KDcraw_FOUND TRUE)            
        endif()

        set(KF5KDcraw_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extra/libkdcraw/src ${CMAKE_BINARY_DIR}/extra/libkdcraw/src ${CMAKE_BINARY_DIR}/extra/libkdcraw)
        set(KF5KDcraw_LIBRARIES KF5KDcraw)

    endif()

    message(STATUS "libkdcraw found       : ${KF5KDcraw_FOUND}")
    message(STATUS "libkdcraw library     : ${KF5KDcraw_LIBRARIES}")
    message(STATUS "libkdcraw includes    : ${KF5KDcraw_INCLUDE_DIRS}")

endmacro()

###########################################################################################################################################"

macro(DETECT_LIBKEXIV2 MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKEXIV2)

        message(STATUS "libkexiv2 : search system based library")
        find_package(KF5KExiv2 ${MIN_VERSION})

        if(KF5KExiv2_FOUND)
            set(KF5KExiv2_LIBRARIES KF5::KExiv2)
            get_target_property(KF5KExiv2_INCLUDE_DIRS KF5::KExiv2 INTERFACE_INCLUDE_DIRECTORIES)
            set(KF5KExiv2_FOUND TRUE)
        else()
            set(KF5KExiv2_FOUND FALSE)
        endif()

    else()

        message(STATUS "libkexiv2 : use local library from ${CMAKE_SOURCE_DIR}/extra/libkexiv2/")
        find_file(KF5KExiv2_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkexiv2/)

        if(NOT KF5KExiv2_FOUND)
            message(ERROR "libkexiv2 : local library not found")
            set(KF5KExiv2_FOUND FALSE)
        else()
            set(KF5KExiv2_FOUND TRUE) 
        endif()

        set(KF5KExiv2_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extra/libkexiv2/src ${CMAKE_BINARY_DIR}/extra/libkexiv2/src ${CMAKE_BINARY_DIR}/extra/libkexiv2)
        set(KF5KExiv2_LIBRARIES KF5KExiv2)

    endif()

    message(STATUS "libkexiv2 found       : ${KF5KExiv2_FOUND}")
    message(STATUS "libkexiv2 library     : ${KF5KExiv2_LIBRARIES}")
    message(STATUS "libkexiv2 includes    : ${KF5KExiv2_INCLUDE_DIRS}")

endmacro()

###########################################################################################################################################"

macro(DETECT_LIBKFACE MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKFACE)

        message(STATUS "libkface : search system based library")
        find_package(KF5KFace ${MIN_VERSION})

        if(KF5KFace_FOUND)
            set(KF5KFace_LIBRARIES KF5::KFace)
            get_target_property(KF5KFace_INCLUDE_DIRS KF5::KFace INTERFACE_INCLUDE_DIRECTORIES)
            set(KF5KFace_FOUND TRUE)
        else()
            set(KF5KFace_FOUND FALSE)
        endif()

    else()

        message(STATUS "libkface : use local library from ${CMAKE_SOURCE_DIR}/extra/libkface/")
        find_file(KF5KFace_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkface/)

        if(NOT KF5KFace_FOUND)
            message(ERROR "libkface : local library not found")
            set(KF5KFace_FOUND FALSE)
        else()
            set(KF5KFace_FOUND TRUE) 
        endif()

        set(KF5KFace_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extra/libkface/src ${CMAKE_BINARY_DIR}/extra/libkface/src ${CMAKE_BINARY_DIR}/extra/libkface)
        set(KF5KFace_LIBRARIES KF5KFace)

    endif()

    message(STATUS "libkface found        : ${KF5KFace_FOUND}")
    message(STATUS "libkface library      : ${KF5KFace_LIBRARIES}")
    message(STATUS "libkface includes     : ${KF5KFace_INCLUDE_DIRS}")

endmacro()

###########################################################################################################################################"

macro(DETECT_LIBKGEOMAP MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKGEOMAP)

        message(STATUS "libkgeomap : search system based library")
        find_package(KF5KGeoMap ${MIN_VERSION})

        if(KF5KGeoMap_FOUND)
            set(KF5KGeoMap_LIBRARIES KF5::KGeoMap)
            get_target_property(KF5KGeoMap_INCLUDE_DIRS KF5::KGeoMap INTERFACE_INCLUDE_DIRECTORIES)
            set(KF5KGeoMap_FOUND TRUE)
        else()
            set(KF5KGeoMap_FOUND FALSE)
        endif()

    else()

        message(STATUS "libkgeomap : use local library from ${CMAKE_SOURCE_DIR}/extra/libkgeomap/")
        find_file(KF5KGeoMap_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkgeomap/)

        if(NOT KF5KGeoMap_FOUND)
            message(ERROR "libkgeomap : local library not found")
            set(KF5KGeoMap_FOUND FALSE)
        else()
            set(KF5KGeoMap_FOUND TRUE) 
        endif()

        set(KF5KGeoMap_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/extra/libkgeomap/src ${CMAKE_BINARY_DIR}/extra/libkgeomap)
        set(KF5KGeoMap_LIBRARIES KF5KGeoMap)

    endif()

    message(STATUS "libkgeomap found      : ${KF5KGeoMap_FOUND}")
    message(STATUS "libkgeomap library    : ${KF5KGeoMap_LIBRARIES}")
    message(STATUS "libkgeomap includes   : ${KF5KGeoMap_INCLUDE_DIRS}")

endmacro()

###########################################################################################################################################"

macro(DETECT_LIBKVKONTAKTE MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKVKONTAKTE)

        message(STATUS "libkvkontakte : search system based library")
        find_package(KF5Vkontakte ${MIN_VERSION})

        if(KF5Vkontakte_FOUND)
            set(LIBKVKONTAKTE_LIBRARIES KF5::Vkontakte)
            get_target_property(LIBKVKONTAKTE_INCLUDES KF5::Vkontakte INTERFACE_INCLUDE_DIRECTORIES)
            set(KF5Vkontakte_FOUND TRUE)
        else()
            set(KF5Vkontakte_FOUND FALSE)
        endif()

    else()

        message(STATUS "libkvkontakte : use local library from ${CMAKE_SOURCE_DIR}/extra/libkvkontakte/")
        find_file(KF5Vkontakte_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkvkontakte/)

        if(NOT KF5Vkontakte_FOUND)
            message(ERROR "libkvkontakte : local library not found")
            set(KF5Vkontakte_FOUND FALSE)
        else()
            set(KF5Vkontakte_FOUND TRUE)
        endif()

        set(LIBKVKONTAKTE_INCLUDES ${CMAKE_SOURCE_DIR}/extra/libkvkontakte/ ${CMAKE_BINARY_DIR}/extra/libkvkontakte)
        set(LIBKVKONTAKTE_LIBRARIES KF5Vkontakte)

    endif()

    message(STATUS "libkvkontakte found      : ${KF5Vkontakte_FOUND}")
    message(STATUS "libkvkontakte library    : ${LIBKVKONTAKTE_LIBRARIES}")
    message(STATUS "libkvkontakte includes   : ${LIBKVKONTAKTE_INCLUDES}")

endmacro()

###########################################################################################################################################"

macro(DETECT_LIBMEDIAWIKI MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBMEDIAWIKI)

        message(STATUS "libmediawiki : search system based library")
        find_package(KF5MediaWiki ${MIN_VERSION})

        if(KF5MediaWiki_FOUND)
            set(LIBMEDIAWIKI_LIBRARIES KF5::MediaWiki)
            get_target_property(LIBMEDIAWIKI_INCLUDES KF5::MediaWiki INTERFACE_INCLUDE_DIRECTORIES)
            set(KF5MediaWiki_FOUND TRUE)
        else()
            set(KF5MediaWiki_FOUND FALSE)
        endif()

    else()

        message(STATUS "libmediawiki : use local library from ${CMAKE_SOURCE_DIR}/extra/libmediawiki/")
        find_file(KF5MediaWiki_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libmediawiki/)

        if(NOT KF5MediaWiki_FOUND)
            message(ERROR "libmediawiki : local library not found")
            set(KF5MediaWiki_FOUND FALSE)
        else()
            set(KF5MediaWiki_FOUND TRUE)
        endif()

        set(LIBMEDIAWIKI_INCLUDES ${CMAKE_SOURCE_DIR}/extra/libmediawiki/ ${CMAKE_BINARY_DIR}/extra/libmediawiki)
        set(LIBMEDIAWIKI_LIBRARIES KF5MediaWiki)

    endif()

    message(STATUS "libmediawiki found      : ${KF5MediaWiki_FOUND}")
    message(STATUS "libmediawiki library    : ${LIBMEDIAWIKI_LIBRARIES}")
    message(STATUS "libmediawiki includes   : ${LIBMEDIAWIKI_INCLUDES}")

endmacro()
