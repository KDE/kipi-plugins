#
# Copyright (c) 2010-2016 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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
set(KIPIPLUGINS_MINOR_VERSION "0")
set(KIPIPLUGINS_PATCH_VERSION "0")

# kipi-plugins release date
set(KIPIPLUGINS_RELEASE_DATE "2016-05-29")

# Suffix to add at end of version string. Usual values are:
# "-git"   : alpha code unstable from git. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
set(KIPIPLUGINS_SUFFIX_VERSION "-beta7")

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
set(ECM_MIN_VERSION      "1.5.0")
set(KF5_MIN_VERSION      "5.1.0")
set(QT_MIN_VERSION       "5.2.0")
set(KIPI_MIN_VERSION     "5.0.0")
cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

############## ECM setup ######################

find_package(ECM ${ECM_MIN_VERSION} CONFIG REQUIRED)
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
include(ECMPackageConfigHelpers)
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
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS
                 Test)
endif()

find_package(KF5 ${KF5_MIN_VERSION}
             REQUIRED COMPONENTS
             Config
             WindowSystem
             XmlGui
             I18n
             Service      # for SendImages (KToolInvocation)
)

find_package(KF5 ${KF5_MIN_VERSION}
             OPTIONAL_COMPONENTS
             IconThemes   # for GoogleServices (KIO/PixmapLoader)
             Archive      # for FlashExport
             KIO          # Some kipi-plugins depends of KIO indeep..
)

# Check if KIO have been compiled with KIOWidgets. Some tools rely only to KIO core, some other with KIOWidgets too..
if(KF5KIO_FOUND)
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

DETECT_LIBKIPI(${KIPI_MIN_VERSION})

if(KF5Kipi_FOUND)

    # -- Optional dependencies detection required by some plugins -------------------------------------

    DETECT_LIBMEDIAWIKI(5.0.0)                                                 # For Mediawiki.
    DETECT_LIBKVKONTAKTE(4.70.0)                                               # For VKontakte.
    find_package(QtKOAuth)                                                     # For Imgur Auhtenticated API.

    # ==================================================================================================
    # Log messages

    message(STATUS "")
    message(STATUS "----------------------------------------------------------------------------------")
    message(STATUS " ${PROJECT_NAME} ${KIPIPLUGINS_VERSION_STRING} dependencies results   <http://www.digikam.org>")
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
    PRINT_OPTIONAL_LIBRARY_STATUS("libqtkoauth"     "https://github.com/kypeli/kQOAuth"                               "(version >= 0.97)"                    "Kipi-plugins will be compiled without libqtkoauth support."     QTKOAUTH_FOUND)

    # Optional plugins results ----------------------------------------------------------------------------

    message(STATUS "")

    if(Qt5_FOUND AND KF5_FOUND AND KF5Kipi_FOUND)
        set(KIPI_COMPILE_MESSAGE "kipi-plugins will be compiled")
        FILL_WITH_DOTS(KIPI_COMPILE_MESSAGE)
        message(STATUS " ${KIPI_COMPILE_MESSAGE} YES")
        set(KIPIPLUGINS_CAN_BE_COMPILED true)
    else()
        message(FATAL_ERROR " kipi-plugins will not be compiled! Look README file for more details about dependencies...")
        set(KIPIPLUGINS_CAN_BE_COMPILED false)
    endif()

    PRINT_COMPONENT_COMPILE_STATUS("VKontakte"          KF5Vkontakte_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("Mediawiki"          KF5MediaWiki_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("FlashExport"        KF5Archive_FOUND)

    # ==================================================================================================

    if(KIPIPLUGINS_CAN_BE_COMPILED)

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
                            ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/tools/imageio
                            ${KF5Kipi_INCLUDE_DIRS}
                        )

        # To prevent warnings from M$ compiler

        if(WIN32 AND MSVC)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            add_definitions(-D_ATL_SECURE_NO_WARNINGS)
            add_definitions(-D_AFX_SECURE_NO_WARNINGS)
        endif()

        if(NOT WIN32)
            configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/kipi-plugins.lsm.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipi-plugins.lsm)
        endif()

        add_subdirectory(common)
        add_subdirectory(printimages)
        add_subdirectory(kmlexport)
        add_subdirectory(sendimages)
        add_subdirectory(flickr)
        add_subdirectory(dropbox)

        if(KF5Archive_FOUND)
            add_subdirectory(flashexport)
        endif()

        if(KF5Vkontakte_FOUND)
            add_subdirectory(vkontakte)
        endif()

        if(KF5MediaWiki_FOUND)
            add_subdirectory(mediawiki)
        endif()

        if(KF5KIO_FOUND)

            # Tools that only depend of KIO core. Code relying to KIO is listed on the right side.

            add_subdirectory(imgur)                # imgurtalker.cpp
            add_subdirectory(imageshack)           # imageshacktalker.cpp
            add_subdirectory(yandexfotki)          # yftalker.cpp


            # Tools which need KIO Widgets too.

            if(KF5KIOWidgets_FOUND)

                add_subdirectory(facebook)         # fbtalker.cpp
                add_subdirectory(piwigo)           # piwigotalker.cpp
                add_subdirectory(rajce)            # rajcesession.cpp
                add_subdirectory(smug)             # smugtalker.cpp
                add_subdirectory(remotestorage)    # kioimportwindow.cpp, kioexportwindow.cpp
                add_subdirectory(googleservices)   # autorize.cpp, gptalker.cpp, replacedialog.cpp, gdtalker.cpp, gswindow.cpp

           endif()

        endif()


        if(BUILD_TESTING)
            add_subdirectory(tests)
        endif()

    endif()

else()

    message(STATUS "")
    message(STATUS "----------------------------------------------------------------------------------")
    message(STATUS " ${PROJECT_NAME} ${KIPIPLUGINS_VERSION_STRING} will not be compiled")
    PRINT_OPTIONAL_LIBRARY_STATUS("libkipi"   "https://projects.kde.org/projects/kde/kdegraphics/libs/libkipi"   "(version >= ${KIPI_MIN_VERSION}))" "Kipi-plugins needs this library to be compiled." KF5Kipi_FOUND)
    message(STATUS "")

endif()
