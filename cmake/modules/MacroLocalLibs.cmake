# Some useful macros to detect local or system based libraries
#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro(DETECT_LIBKIPI MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKIPI)

        message(STATUS "libkipi : search system based library")
        find_package(KF5Kipi ${MIN_VERSION})

        if(KF5Kipi_FOUND)
            set(KF5Kipi_LIBRARIES KF5::Kipi)
            get_target_property(KF5Kipi_INCLUDE_DIRS KF5::Kipi INTERFACE_INCLUDE_DIRECTORIES)
            set(KF5Kipi_FOUND TRUE)
        else()
            set(KF5Kipi_FOUND FALSE)
        endif()

    else()

        message(STATUS "libkipi : use local library from ${CMAKE_SOURCE_DIR}/extra/libkipi/")

        if(EXISTS "${CMAKE_SOURCE_DIR}/extra/libkipi/CMakeLists.txt")
            set(KF5Kipi_FOUND TRUE)
        else()
            message(WARNING "libkipi : local library not found")
            set(KF5Kipi_FOUND FALSE)
        endif()

        set(KF5Kipi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extra/libkipi/src ${CMAKE_BINARY_DIR}/extra/libkipi/src ${CMAKE_BINARY_DIR}/extra/libkipi)
        set(KF5Kipi_LIBRARIES KF5Kipi)

    endif()

    message(STATUS "libkipi found         : ${KF5Kipi_FOUND}")
    message(STATUS "libkipi library       : ${KF5Kipi_LIBRARIES}")
    message(STATUS "libkipi includes      : ${KF5Kipi_INCLUDE_DIRS}")

    if(${KF5Kipi_FOUND})

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

    endif()

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

        if(EXISTS "${CMAKE_SOURCE_DIR}/extra/libkvkontakte/CMakeLists.txt")
            set(KF5Vkontakte_FOUND TRUE)
        else()
            message(WARNING "libkvkontakte : local library not found")
            set(KF5Vkontakte_FOUND FALSE)
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

        if(EXISTS "${CMAKE_SOURCE_DIR}/extra/libmediawiki/CMakeLists.txt")
            set(KF5MediaWiki_FOUND TRUE)
        else()
            message(WARNING "libmediawiki : local library not found")
            set(KF5MediaWiki_FOUND FALSE)
        endif()

        set(LIBMEDIAWIKI_INCLUDES ${CMAKE_SOURCE_DIR}/extra/libmediawiki/ ${CMAKE_BINARY_DIR}/extra/libmediawiki)
        set(LIBMEDIAWIKI_LIBRARIES KF5MediaWiki)

    endif()

    message(STATUS "libmediawiki found      : ${KF5MediaWiki_FOUND}")
    message(STATUS "libmediawiki library    : ${LIBMEDIAWIKI_LIBRARIES}")
    message(STATUS "libmediawiki includes   : ${LIBMEDIAWIKI_INCLUDES}")

endmacro()
