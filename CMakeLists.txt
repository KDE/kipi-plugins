#
# Copyright (c) 2010-2018 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

project(kipi-plugins)

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "Starting CMake configuration for: ${PROJECT_NAME}")

# =======================================================
# Information to update before to release this package.

# kipi-plugins version
set(KIPIPLUGINS_MAJOR_VERSION "5")
set(KIPIPLUGINS_MINOR_VERSION "9")
set(KIPIPLUGINS_PATCH_VERSION "1")

# Suffix to add at end of version string. Usual values are:
# "-git"   : alpha code unstable from git. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
set(KIPIPLUGINS_SUFFIX_VERSION "")

# =======================================================
# Set env. variables accordinly.

set(KIPIPLUGINS_VERSION_STRING
    "${KIPIPLUGINS_MAJOR_VERSION}.${KIPIPLUGINS_MINOR_VERSION}.${KIPIPLUGINS_PATCH_VERSION}${KIPIPLUGINS_SUFFIX_VERSION}"
)

# NOTE: This string is used to set libkipiplugins SO version ID
set(KIPIPLUGINS_LIB_SO_VERSION_STRING
    "${KIPIPLUGINS_MAJOR_VERSION}.${KIPIPLUGINS_MINOR_VERSION}.${KIPIPLUGINS_PATCH_VERSION}"
)

# =======================================================

set(CMAKE_MIN_VERSION    "3.0.0")
set(KF5_MIN_VERSION      "5.18.0")
set(QT_MIN_VERSION       "5.6.0")
set(KIPI_MIN_VERSION     "5.0.0")
set(MEDIAWIKI_MIN_VERSION "5.0.0")
cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

############## ECM setup ######################

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${kipi-plugins_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

# Cmake macros
include(GenerateExportHeader)
include(CheckFunctionExists)
include(FeatureSummary)
# ECM macros
include(ECMOptionalAddSubdirectory)
include(ECMAddTests)
include(ECMMarkNonGuiExecutable)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMSetupVersion)
include(ECMInstallIcons)
include(ECMAddAppIcon)
# KDE macros
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)
include(KDECompilerSettings)
# Local macros
include(MacroUtils)
include(MacroLocalLibs)

##########################################################################

option(ENABLE_LEGACY "Build Kipi-plugins with legacy plugins support (default=ON)" ON)

############## Find Packages ###################

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS
             Core
             Widgets
             PrintSupport
             Gui
             Xml
             XmlPatterns
             Svg
             Concurrent
             Network
)

if(BUILD_TESTING)
    find_package(Qt5Test ${QT_MIN_VERSION} REQUIRED NO_MODULE)
endif()

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
             Config
             WindowSystem
             XmlGui
             I18n
)

find_package(KF5Archive ${KF5_MIN_VERSION}) # for FlashExport

find_package(KF5KIO ${KF5_MIN_VERSION})

if(KF5KIO_FOUND)
    # Some tools rely only on KIO core, others need KIOWidgets too..
    get_target_property(KIOWidgets_INCLUDE_DIRS KF5::KIOWidgets INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "KF5::KIOWidgets include dirs: ${KIOWidgets_INCLUDE_DIRS}")

    if(NOT KIOWidgets_INCLUDE_DIRS)
        message(STATUS "KF5::KIOWidgets not available in shared KIO library. Some tools will not be compiled.")
        set(KF5KIOWidgets_FOUND FALSE)
    else()
        set(KF5KIOWidgets_FOUND TRUE)
    endif()
endif()

# Dependencies detection required by all plugins

find_package(KF5Kipi ${KIPI_MIN_VERSION} REQUIRED)

get_target_property(KF5Kipi_INCLUDE_DIRS KF5::Kipi INTERFACE_INCLUDE_DIRECTORIES)

# Detect libkipi so version used to compile kipi tool to identify if plugin can be loaded in memory by libkipi.
# This will be used to populate plugin desktop files.

foreach(var ${KF5Kipi_INCLUDE_DIRS})
    if(EXISTS "${var}/libkipi_config.h")
        set(KF5KipiConfig_FOUND "${var}/libkipi_config.h")
        message(STATUS "Libkipi config header: ${KF5KipiConfig_FOUND}")
        break()
    endif()
endforeach()

if(KF5KipiConfig_FOUND)
    file(READ ${KF5KipiConfig_FOUND} KIPI_CONFIG_H_CONTENT)

    string(REGEX REPLACE
            ".*static +const +int +kipi_binary_version += ([^ ;]+).*"
            "\\1"
            KIPI_LIB_SO_CUR_VERSION_FOUND
            "${KIPI_CONFIG_H_CONTENT}"
    )

    set(KIPI_LIB_SO_CUR_VERSION ${KIPI_LIB_SO_CUR_VERSION_FOUND} CACHE STRING "libkipi so version")
else()
    message(FATAL_ERROR "Could not find libkipi SO version")
    set(KF5Kipi_FOUND FALSE)
endif()

message(STATUS "libkipi includes      : ${KF5Kipi_INCLUDE_DIRS}")
message(STATUS "libkipi SO version    : ${KIPI_LIB_SO_CUR_VERSION}")

# -- Optional dependencies detection required by some plugins -------------------------------------

find_package(KF5MediaWiki ${MEDIAWIKI_MIN_VERSION})
DETECT_LIBKVKONTAKTE(4.70.0)                                               # For VKontakte.

# ==================================================================================================
# Log messages

message(STATUS "")
message(STATUS "----------------------------------------------------------------------------------")
message(STATUS " ${PROJECT_NAME} ${KIPIPLUGINS_VERSION_STRING} dependencies results")
message(STATUS "")

# Require shared libraries results.

PRINT_LIBRARY_STATUS("libqt"     "http://www.qtsoftware.com"                                        "(version >= ${QT_MIN_VERSION})"     Qt5_FOUND)
PRINT_LIBRARY_STATUS("libkde"    "http://www.kde.org"                                               "(version >= ${KF5_MIN_VERSION})"    KF5_FOUND)
PRINT_LIBRARY_STATUS("libkipi"   "https://projects.kde.org/projects/kde/kdegraphics/libs/libkipi"   "(version >= ${KIPI_MIN_VERSION}))"  KF5Kipi_FOUND)

# Optional shared libraries results.

PRINT_OPTIONAL_LIBRARY_STATUS("libkio"          "https://projects.kde.org/projects/frameworks/kio"                "(version >= ${KF5_MIN_VERSION})"      "Kipi-plugins will be compiled without libkio support."          KF5KIO_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkarchive"     "https://projects.kde.org/projects/frameworks/karchive"           "(version >= ${KF5_MIN_VERSION})"      "Kipi-plugins will be compiled without libkarchive support."     KF5Archive_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libmediawiki"    "https://projects.kde.org/projects/extragear/libs/libmediawiki"   "(version >= ${KF5_MIN_VERSION})"      "Kipi-plugins will be compiled without libmediawiki support."    KF5MediaWiki_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkvkontakte"   "https://projects.kde.org/projects/extragear/libs/libkvkontakte"  "(version >= 4.70.0)"                  "Kipi-plugins will be compiled without libkvkontakte support."   KF5Vkontakte_FOUND)

# Optional plugins results ----------------------------------------------------------------------------

PRINT_COMPONENT_COMPILE_STATUS("Legacy plugins"     ENABLE_LEGACY)
PRINT_COMPONENT_COMPILE_STATUS("VKontakte"          KF5Vkontakte_FOUND)
PRINT_COMPONENT_COMPILE_STATUS("Mediawiki"          KF5MediaWiki_FOUND)
PRINT_COMPONENT_COMPILE_STATUS("FlashExport"        KF5Archive_FOUND)

# ==================================================================================================
# Create git version header

# We only do this IF we are in a .git dir
find_file(GIT_MARKER entries PATHS ${CMAKE_SOURCE_DIR}/.git)

if(NOT GIT_MARKER)
    set (GIT_MARKER ${CMAKE_SOURCE_DIR}/CMakeLists.txt)  # Dummy file
endif()

# Add a custom command to drive the git script whenever the git entries
# file changes.
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/gitscript.cmake.in"
                "${CMAKE_CURRENT_BINARY_DIR}/gitscript.cmake"
                @ONLY)

# Add a custom target to drive the custom command.
add_custom_target(kipiplugins-gitversion
                    ALL COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/gitscript.cmake")

# ==================================================================================================

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins
                    ${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins
                    ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/dialogs
                    ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/widgets
                    ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/tools
                    ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/o2/src
                    ${KF5Kipi_INCLUDE_DIRS}
                )

# To prevent warnings from M$ compiler

if(WIN32 AND MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_ATL_SECURE_NO_WARNINGS)
    add_definitions(-D_AFX_SECURE_NO_WARNINGS)
endif()

# Remove pedantic GCC flag which generate a lots of warnings on the console with qCDebug()
while(CMAKE_CXX_FLAGS MATCHES "-pedantic")
    string(REPLACE "-pedantic" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endwhile()

# Remove Wdate-time GCC flag which generate a lots of compile warnings
while(CMAKE_CXX_FLAGS MATCHES "-Wdate-time")
    string(REPLACE "-Wdate-time" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endwhile()

add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

add_subdirectory(common)
add_subdirectory(flickr)
add_subdirectory(dropbox)
add_subdirectory(facebook)
add_subdirectory(imgur)
add_subdirectory(piwigo)
add_subdirectory(rajce)
add_subdirectory(smug)
add_subdirectory(imageshack)
add_subdirectory(yandexfotki)
add_subdirectory(googleservices)

if(ENABLE_LEGACY)
    add_subdirectory(printimages)
    add_subdirectory(kmlexport)
    add_subdirectory(sendimages)
    add_subdirectory(jalbum)

    if(KF5Archive_FOUND)
        add_subdirectory(flashexport)
    endif()

endif()

if(KF5Vkontakte_FOUND)
    add_subdirectory(vkontakte)
endif()

if(KF5MediaWiki_FOUND)
    add_subdirectory(mediawiki)
endif()

if(KF5KIO_FOUND AND KF5KIOWidgets_FOUND)
    add_subdirectory(remotestorage)    # kioimportwindow.cpp, kioexportwindow.cpp
endif()
