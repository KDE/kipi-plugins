#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

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
set(KIPIPLUGINS_RELEASE_DATE "2015-09-06")

# Suffix to add at end of version string. Usual values are:
# "-git"   : alpha code unstable from git. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
set(KIPIPLUGINS_SUFFIX_VERSION "-git")

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

set(CMAKE_MIN_VERSION   "2.8.12")
set(ECM_MIN_VERSION     "1.7.0")
set(KF5_VERSION         "5.1.0")
set(REQUIRED_QT_VERSION "5.2.0")
set(KIPI_MIN_VERSION    "5.0.0")
set(KEXIV2_MIN_VERSION  "5.0.0")
set(KDCRAW_MIN_VERSION  "5.0.0")
set(KGEOMAP_MIN_VERSION "5.0.0")
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

find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS
             Core
             Widgets
             Gui
             Xml
             Test
             Svg
             Concurrent
             OPTIONAL_COMPONENTS OpenGL
)

find_package(KF5 REQUIRED COMPONENTS
             KDELibs4Support
             Parts
             Wallet
             KHtml
             Config
             ItemModels
             ThreadWeaver
             I18n
             OPTIONAL_COMPONENTS CalendarCore # for Calendar
)

# Dependencies detection required by all plugins

DETECT_LIBKIPI(${KIPI_MIN_VERSION})

if(KF5Kipi_FOUND)

    DETECT_LIBKEXIV2(${KEXIV2_MIN_VERSION})
    DETECT_LIBKDCRAW(${KDCRAW_MIN_VERSION})
    find_package(JPEG REQUIRED)
    find_package(PNG  REQUIRED)
    find_package(TIFF REQUIRED)

    # -- Optional dependencies detection required by some plugins -------------------------------------

    DETECT_LIBKGEOMAP(5.0.0)                                                   # For Geolocator.
    DETECT_LIBMEDIAWIKI(5.0.0)                                                 # For Mediawiki.
    DETECT_LIBKVKONTAKTE(4.70.0)                                               # For VKontakte.
    DETECT_LIBKSANE(5.0.0)                                                     # For AcquireImages.
    find_package(EXPAT)                                                        # For DNGConverter: XMP SDK need Expat library to compile.
    find_package(Threads)                                                      # For DNGConverter: DNG SDK need native threads support.
    find_package(OpenGL)                                                       # For AdvancedSlideshow and ImageViewer.
    find_package(FLEX)                                                         # For Panorama (must be found before KdepimLibs for MacOS).
    find_package(BISON)                                                        # For Panorama (must be found before KdepimLibs for MacOS).
    find_package(Phonon4Qt5)                                                   # For AdvancedSlideShow.
    find_package(QJSON)                                                        # For Debian Screenshots Facebook, Google Services, Dropbox and Imgur.

#    find_package(Qca-qt5)                                                      # For Shwup.
#    MACRO_OPTIONAL_FIND_PACKAGE(LibXml2)                                       # For Htmlexport.
#    MACRO_OPTIONAL_FIND_PACKAGE(LibXslt)                                       # For Htmlexport.
#    MACRO_OPTIONAL_FIND_PACKAGE(ImageMagick COMPONENTS MagickCore display)     # For VideoSlideshow.
#    MACRO_OPTIONAL_FIND_PACKAGE(QtGStreamer)                                   # For VideoSlideshow.
#    MACRO_OPTIONAL_FIND_PACKAGE(QtKOAuth)                                      # For Imgur Auhtenticated API

    # -- X11 check, LINUX Only -------------------------------------------------------------------------

    if(NOT WIN32 AND NOT APPLE)
        find_package(X11)                                                      # For AdvancedSlideshow and ImageViewer.
    endif()

#     # -- OpenCV ----------------------------------------------------------------------------------------
#
#     include(MacroUtils)
#     include(MacroOpenCV)
#     set(OPENCV_MIN_VERSION "2.4.9")
#     DETECT_OPENCV(${OPENCV_MIN_VERSION} core highgui objdetect contrib legacy imgproc)

    # ==================================================================================================
    # Log messages

    message(STATUS "")
    message(STATUS "----------------------------------------------------------------------------------")
    message(STATUS " ${PROJECT_NAME} ${KIPIPLUGINS_VERSION_STRING} dependencies results   <http://www.digikam.org>")
    message(STATUS "")

    # Require shared libraries results.

    PRINT_LIBRARY_STATUS("libqt"     "http://www.qtsoftware.com"                                        "(version >= ${QT_MIN_VERSION})"     Qt5_FOUND)
    PRINT_LIBRARY_STATUS("libkde"    "http://www.kde.org"                                               "(version >= ${KDE_MIN_VERSION})"    KF5_FOUND)
    PRINT_LIBRARY_STATUS("libjpeg"   "http://www.ijg.org"                                               "(version >= 6b)"                    JPEG_FOUND)
    PRINT_LIBRARY_STATUS("libtiff"   "http://www.remotesensing.org/libtiff"                             "(version >= 3.8.2)"                 TIFF_FOUND)
    PRINT_LIBRARY_STATUS("libpng"    "http://www.libpng.org/pub/png/libpng.html"                        "(version >= 1.2.7)"                 PNG_FOUND)
    PRINT_LIBRARY_STATUS("libkipi"   "https://projects.kde.org/projects/kde/kdegraphics/libs/libkipi"   "(version >= ${KIPI_MIN_VERSION}))"  KF5Kipi_FOUND)
    PRINT_LIBRARY_STATUS("libkexiv2" "https://projects.kde.org/projects/kde/kdegraphics/libs/libkexiv2" "(version >= ${KEXIV2_MIN_VERSION})" KF5KExiv2_FOUND)
    PRINT_LIBRARY_STATUS("libkdcraw" "https://projects.kde.org/projects/kde/kdegraphics/libs/libkdcraw" "(version >= ${KDCRAW_MIN_VERSION})" KF5KDcraw_FOUND)

    # Optional shared libraries results.

    PRINT_OPTIONAL_LIBRARY_STATUS("libkgeomap"     "https://projects.kde.org/projects/extragear/libs/libkgeomap"     "(version >= ${KGEOMAP_MIN_VERSION})" "Kipi-plugins will be compiled without libkgeomap support."     KF5KGeoMap_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libmediawiki"   "https://projects.kde.org/projects/extragear/libs/libmediawiki"   "(version >= 5.0.0)"                  "Kipi-plugins will be compiled without libmediawiki support."   KF5MediaWiki_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libkvkontakte"  "https://projects.kde.org/projects/extragear/libs/libkvkontakte"  "(version >= 4.70.0)"                 "Kipi-plugins will be compiled without libkvkontakte support."  KF5Vkontakte_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libksane"       "https://projects.kde.org/projects/kde/kdegraphics/libs/libksane" "(version >= 5.0.0)"                  "Kipi-plugins will be compiled without libksane support."       KF5Sane_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libkcalcore"    "https://projects.kde.org/projects/kde/pim/kcalcore"              "(version >= 4.81.0)"                 "Kipi-plugins will be compiled without libkcalcore support."    KF5CalendarCore_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libexpat"       "http://expat.sourceforge.net"                                    "(version >= 2.0.0)"                  "Kipi-plugins will be compiled without libexpat support."       EXPAT_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libqjson"       "http://qjson.sourceforge.net"                                    "(version >= 0.8.1)"                  "Kipi-plugins will be compiled without libqjson support."       QJSON_FOUND)

#     PRINT_OPTIONAL_LIBRARY_STATUS("libqca"         "http://delta.affinix.com/qca"                                    "(version >= 2.1.0"                   "Kipi-plugins will be compiled without libqca support."         Qca-qt5_FOUND)
#     PRINT_OPTIONAL_LIBRARY_STATUS("libxml2"        "http://xmlsoft.org"                                              "(version >= 2.7.0)"                  "Kipi-plugins will be compiled without libxml2 support."        LIBXML2_FOUND)
#     PRINT_OPTIONAL_LIBRARY_STATUS("libxslt"        "http://xmlsoft.org/XSLT"                                         "(version >= 1.1.0)"                  "Kipi-plugins will be compiled without libxstl support."        LIBXSLT_FOUND)
#     PRINT_OPTIONAL_LIBRARY_STATUS("libopencv"      "http://opencv.willowgarage.com"                                  "(version >= ${OPENCV_MIN_VERSION})"  "Kipi-plugins will be compiled without libopencv support."      OpenCV_FOUND)
#     PRINT_OPTIONAL_LIBRARY_STATUS("libimagemagick" "http://www.imagemagick.org"                                      "(version >= 5.5.4)"                  "Kipi-plugins will be compiled without libimagemagick support." ImageMagick_FOUND)
#     PRINT_OPTIONAL_LIBRARY_STATUS("libqtgstreamer" "http://gstreamer.freedesktop.org/modules/qt-gstreamer.html"      "(version >= 0.9.0)"                  "Kipi-plugins will be compiled without libqtgstreamer support." QTGSTREAMER_FOUND)
#     PRINT_OPTIONAL_LIBRARY_STATUS("libqtkoauth"    "http://gitorious.org/kqoauth"                                    "(version >= 0.97)"                   "Kipi-plugins will be compiled without libqtkoauth support."    QTKOAUTH_FOUND)

    PRINT_OPTIONAL_LIBRARY_STATUS("native threads support" "" "" "Kipi-plugins will be compiled without native threads support." CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
    PRINT_OPTIONAL_LIBRARY_STATUS("native Opengl support"  "" "" "Kipi-plugins will be compiled without native OpenGL support."  OPENGL_FOUND AND OPENGL_GLU_FOUND)
    PRINT_OPTIONAL_QTMODULE_STATUS("Qt5 OpenGL"            Qt5OpenGL_FOUND)

    # LINUX Only.
    if(NOT WIN32 AND NOT APPLE)
        PRINT_OPTIONAL_LIBRARY_STATUS("libX11" "http://www.x11.org" "(version >= 1.1.5)" "Kipi-plugins will be compiled without libX11 support." X11_FOUND AND X11_Xrandr_FOUND)
    endif()

    # Optional plugins results ----------------------------------------------------------------------------

     message(STATUS "")

     if(Qt5_FOUND AND KF5_FOUND AND JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KF5Kipi_FOUND AND KF5KDcraw_FOUND AND KF5KExiv2_FOUND)
         set(KIPI_COMPILE_MESSAGE "kipi-plugins will be compiled")
         FILL_WITH_DOTS(KIPI_COMPILE_MESSAGE)
         message(STATUS " ${KIPI_COMPILE_MESSAGE} YES")
         set(KIPIPLUGINS_CAN_BE_COMPILED true)
     else()
         message(FATAL_ERROR " kipi-plugins will not be compiled! Look README file for more details about dependencies...")
         set(KIPIPLUGINS_CAN_BE_COMPILED false)
     endif()

     # decide if AdvancedSlideshow and ImageViewer can be built
     if(NOT WIN32 AND NOT APPLE)
         if(X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND AND Qt5OpenGL_FOUND)
             set(BUILD_VIEWERS TRUE)
         else()
             set(BUILD_VIEWERS FALSE)
         endif()
     else()
         if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND Qt5OpenGL_FOUND)
             set(BUILD_VIEWERS TRUE)
         elseif()
             set(BUILD_VIEWERS FALSE)
         endif()
     endif()

     # Rules about DNGConverter
     if(EXPAT_FOUND)
         if(CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
              set(BUILD_DNGCONVERTER TRUE)
         else()
              set(BUILD_DNGCONVERTER FALSE)
         endif()
     endif()

    PRINT_COMPONENT_COMPILE_STATUS("ImageViewer"        BUILD_VIEWERS)
    PRINT_COMPONENT_COMPILE_STATUS("DNGConverter"       BUILD_DNGCONVERTER)
    PRINT_COMPONENT_COMPILE_STATUS("Panorama"           FLEX_FOUND AND BISON_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("Geolocator"         KF5KGeoMap_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("AcquireImages"      KF5Sane_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("VKontakte"          KF5Vkontakte_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("Mediawiki"          KF5MediaWiki_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("AdvancedSlideshow"  BUILD_VIEWERS)
    PRINT_COMPONENT_COMPILE_STATUS("GoogleServices"     QJSON_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("Calendar"           KF5CalendarCore_FOUND)

#     PRINT_COMPONENT_COMPILE_STATUS("Shwup"              QCA2_FOUND)
#     PRINT_COMPONENT_COMPILE_STATUS("HtmlExport"         LIBXML2_FOUND AND LIBXSLT_FOUND)
#     PRINT_COMPONENT_COMPILE_STATUS("Facebook"           QJSON_FOUND)
#     PRINT_COMPONENT_COMPILE_STATUS("Dropbox"            QJSON_FOUND)
#     PRINT_COMPONENT_COMPILE_STATUS("Imgur"              QJSON_FOUND)
#     PRINT_COMPONENT_COMPILE_STATUS("VideoSlideshow"     ImageMagick_FOUND AND QTGSTREAMER_FOUND)
#     PRINT_COMPONENT_COMPILE_STATUS("RemoveRedEyes"      OpenCV_FOUND)
#     PRINT_COMPONENT_COMPILE_STATUS("Debian Screenshots" QJSON_FOUND)
#
#
#     # Set QtGStreamer API version --------------------------------------------------------------------------
#
#     if(QTGSTREAMER_FOUND)
#
#         message(STATUS "----------------------------------------------------------------------------------")
#         message(STATUS "")
#
#         if((${QtGStreamer_VERSION_MAJOR} EQUAL 1 AND ${QtGStreamer_VERSION_MINOR} EQUAL 1 AND ${QtGStreamer_VERSION_PATCH} GREATER 80)
#            OR (${QtGStreamer_VERSION_MAJOR} EQUAL 1 AND ${QtGStreamer_VERSION_MINOR} GREATER 1) )
#             set(GSTREAMER_API_1 TRUE)
#         else()
#             set(GSTREAMER_API_1 FALSE)
#         endif()
#
#         configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/gstreamerapi.h.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/gstreamerapi.h" @ONLY)
#
#         message(STATUS "Found QtGstreamer version: ${QtGStreamer_VERSION}")
#         message(STATUS "Using GStreamer API 1:     ${GSTREAMER_API_1}")
#
#         message(STATUS "----------------------------------------------------------------------------------")
#         message(STATUS "")
#
#     endif()

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
                            ${KF5KExiv2_INCLUDE_DIRS}
                            ${KF5KDcraw_INCLUDE_DIRS}
                            ${KF5Kipi_INCLUDE_DIRS}
                        )

        # To prevent warnings from M$ compiler

        if(WIN32 AND MSVC)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            add_definitions(-D_ATL_SECURE_NO_WARNINGS)
            add_definitions(-D_AFX_SECURE_NO_WARNINGS)
        endif()

        add_definitions(-DQT_USE_QSTRINGBUILDER -DQT_USE_FAST_OPERATOR_PLUS)

        if(NOT WIN32)
            configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/kipi-plugins.lsm.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipi-plugins.lsm)
        endif()

        add_subdirectory(common)

        if (BUILD_TESTING)
            add_subdirectory(tests)
        endif()

        add_subdirectory(printimages)
        add_subdirectory(kmlexport)
        add_subdirectory(sendimages)
        add_subdirectory(timeadjust)
        add_subdirectory(flashexport)
        add_subdirectory(expoblending)
        add_subdirectory(metadataedit)

        if(KF5Sane_FOUND)
            add_subdirectory(acquireimages)
        endif()

        if(BUILD_VIEWERS)
            add_subdirectory(advancedslideshow)
            add_subdirectory(imageviewer)
        endif()

        if(BUILD_DNGCONVERTER)
            add_subdirectory(dngconverter)
        endif()

        if(KF5KGeoMap_FOUND)
            add_subdirectory(geolocator)
        endif()

        if(FLEX_FOUND AND BISON_FOUND)
            add_subdirectory(panorama)
        endif()

        add_subdirectory(flickr)
        add_subdirectory(smug)
        add_subdirectory(imageshack)
        add_subdirectory(piwigo)
        add_subdirectory(remotestorage)
        add_subdirectory(yandexfotki)
        add_subdirectory(dlnaexport)

        if(KF5Vkontakte_FOUND)
            add_subdirectory(vkontakte)
        endif()

        if(KF5MediaWiki_FOUND)
            add_subdirectory(mediawiki)
        endif()

        if(QJSON_FOUND)
            add_subdirectory(googleservices)
        endif()

        if(KF5CalendarCore_FOUND)
            add_subdirectory(calendar)
        endif()

# Plugins in pending state
#
#         add_subdirectory(rajceexport)
#         add_subdirectory(photolayoutseditor)
#
#         if(Qca-qt5_FOUND)
#             add_subdirectory(shwup)
#         endif()
#
#         if(QJSON_FOUND)
#             add_subdirectory(facebook)
#             add_subdirectory(dropbox)
#             add_subdirectory(debianscreenshots)
#             add_subdirectory(imgurexport)
#         endif()
#
#         # Currently the cmake module distributed w/ cmake uses a different variable
#         # name LIBXLST_EXSLT_LIBRARIES for libxeslt than the cmake modeule 
#         # distributed w/ kdelibs LIBEXSLT_LIBRARIES.
#         if(NOT LIBXSLT_EXSLT_LIBRARIES AND LIBEXSLT_LIBRARIES)
#             set(LIBXSLT_EXSLT_LIBRARIES ${LIBEXSLT_LIBRARIES})
#         endif()
#
#         if(LIBXML2_FOUND AND LIBXSLT_FOUND AND LIBXSLT_EXSLT_LIBRARIES)
#             add_subdirectory(htmlexport)
#         endif()
#
#         if(ImageMagick_FOUND AND QTGSTREAMER_FOUND)
#             add_subdirectory(videoslideshow)
#         endif()
#
#         if(OpenCV_FOUND)
#             add_subdirectory(removeredeyes)
#         endif()

    endif()

else()

    message(STATUS "")
    message(STATUS "----------------------------------------------------------------------------------")
    message(STATUS " libkipi not found. ${PROJECT_NAME} ${KIPIPLUGINS_VERSION_STRING} will not be compiled")
    message(STATUS "")

endif()
