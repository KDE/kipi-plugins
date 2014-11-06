#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

project(kipi-plugins)

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "Starting CMake configuration for: kipi-plugins")

# =======================================================
# Information to update before to release this package.

# kipi-plugins version
set(KIPIPLUGINS_MAJOR_VERSION "4")
set(KIPIPLUGINS_MINOR_VERSION "5")
set(KIPIPLUGINS_PATCH_VERSION "0")

# kipi-plugins release date
set(KIPIPLUGINS_RELEASE_DATE "2014-11-09")

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

cmake_policy(SET CMP0017 NEW)

set(QT_MIN_VERSION   "4.6.0")
set(KDE_MIN_VERSION  "4.4.0")
find_package(Qt4  ${QT_MIN_VERSION}  REQUIRED)
find_package(KDE4 ${KDE_MIN_VERSION} REQUIRED)

include(MacroOptionalFindPackage)
include(FindPackageHandleStandardArgs)
include(KDE4Defaults)
include(MacroLibrary)

# Put local modules first
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

# Dependencies detection required by all plugins

set(KIPI_MIN_VERSION "2.0.0")
find_package(Kipi ${KIPI_MIN_VERSION})

if(KIPI_FOUND)

    find_package(Kexiv2 REQUIRED)
    find_package(JPEG   REQUIRED)
    find_package(PNG    REQUIRED)
    find_package(TIFF   REQUIRED)
    find_package(Kdcraw REQUIRED)

    # -- check the libkdcraw version -----------------------------------------------------------------

    set(KDCRAW_MIN_VERSION "2.2.0")

    if(KDCRAW_VERSION)

        message(STATUS "libkdcraw: Found version ${KDCRAW_VERSION} (required: ${KDCRAW_MIN_VERSION})")
        if (${KDCRAW_VERSION} VERSION_LESS ${KDCRAW_MIN_VERSION})
            set(KDCRAW_FOUND FALSE)
        endif()

    else()

        message(WARNING "libkdcraw: Version information not found, your version is probably too old.")
        set(KDCRAW_FOUND FALSE)

    endif()

    # -- Optional dependencies detection required by some plugins -------------------------------------

    MACRO_OPTIONAL_FIND_PACKAGE(EXPAT)                                       # For DNGConverter: XMP SDK need Expat library to compile.
    MACRO_OPTIONAL_FIND_PACKAGE(Threads)                                     # For DNGConverter: DNG SDK need native threads support.
    MACRO_OPTIONAL_FIND_PACKAGE(LibXml2)                                     # For Htmlexport.
    MACRO_OPTIONAL_FIND_PACKAGE(LibXslt)                                     # For Htmlexport.
    MACRO_OPTIONAL_FIND_PACKAGE(OpenGL)                                      # For AdvancedSlideshow and ImageViewer.
    MACRO_OPTIONAL_FIND_PACKAGE(Ipod)                                        # For ipodexport.
    MACRO_OPTIONAL_FIND_PACKAGE(Gdk)                                         # For ipodexport.
    MACRO_OPTIONAL_FIND_PACKAGE(GLIB2)                                       # For ipodexport.
    MACRO_OPTIONAL_FIND_PACKAGE(GObject)                                     # For ipodexport.
    MACRO_OPTIONAL_FIND_PACKAGE(FLEX)                                        # For Panorama (must be found before KdepimLibs for MacOS).
    MACRO_OPTIONAL_FIND_PACKAGE(BISON)                                       # For Panorama (must be found before KdepimLibs for MacOS).
    MACRO_OPTIONAL_FIND_PACKAGE(KdepimLibs)                                  # For Calendar (libkcal).
    MACRO_OPTIONAL_FIND_PACKAGE(QCA2)                                        # For Shwup and YandexFotki.
    MACRO_OPTIONAL_FIND_PACKAGE(KSane)                                       # For AcquireImages.
    MACRO_OPTIONAL_FIND_PACKAGE(QJSON)                                       # For Debian Screenshots Facebook, GoogleDrive, Dropbox and Imgur.
    MACRO_OPTIONAL_FIND_PACKAGE(KGeoMap)                                     # For GPSSync.
    MACRO_OPTIONAL_FIND_PACKAGE(Mediawiki)                                   # For Mediawiki.
    MACRO_OPTIONAL_FIND_PACKAGE(LibKVkontakte)                               # For VKontakte.
    MACRO_OPTIONAL_FIND_PACKAGE(Hupnp)                                       # For DlnaExport.
    MACRO_OPTIONAL_FIND_PACKAGE(QtSoap)                                      # For DlnaExport.
    MACRO_OPTIONAL_FIND_PACKAGE(ImageMagick COMPONENTS MagickCore display)   # For VideoSlideshow.
    MACRO_OPTIONAL_FIND_PACKAGE(QtGStreamer)                                 # For VideoSlideshow.
    MACRO_OPTIONAL_FIND_PACKAGE(QtKOAuth)                                    # For Imgur Auhtenticated API

    # -- check the libkgeomap version -----------------------------------------------------------------

    set(KGEOMAP_MIN_VERSION "3.0.0")

    if(KGEOMAP_VERSION)

        message(STATUS "libkgeomap: Found version ${KGEOMAP_VERSION} (required: ${KGEOMAP_MIN_VERSION})")
        if(${KGEOMAP_VERSION} VERSION_LESS ${KGEOMAP_MIN_VERSION})
            set(KGEOMAP_FOUND FALSE)
        endif()

    else()

        message(WARNING "libkgeomap: Version information not found, your version is probably too old.")
        set(KGEOMAP_FOUND FALSE)

    endif()

    # -- X11 check, LINUX Only -------------------------------------------------------------------------

    if(NOT WIN32 AND NOT APPLE)
        MACRO_OPTIONAL_FIND_PACKAGE(X11)                                     # For AdvancedSlideshow and ImageViewer.
    endif()

    # -- OpenCV ----------------------------------------------------------------------------------------

    include(MacroUtils)
    include(MacroOpenCV)
    set(OPENCV_MIN_VERSION "2.4.9")
    DETECT_OPENCV(${OPENCV_MIN_VERSION} core highgui objdetect contrib legacy imgproc)

    include(MacroOptionalDependPackage)

    # ==================================================================================================
    # Log messages

    message(STATUS "")
    message(STATUS "----------------------------------------------------------------------------------")
    message(STATUS " kipi-plugins ${KIPIPLUGINS_VERSION_STRING} dependencies results   <http://www.digikam.org>")
    message(STATUS "")

    # Require shared libraries results.

    PRINT_LIBRARY_STATUS("libqt"     "http://www.qtsoftware.com"                                        "(version >= ${QT_MIN_VERSION})"     QT_FOUND)
    PRINT_LIBRARY_STATUS("libkde"    "http://www.kde.org"                                               "(version >= ${KDE_MIN_VERSION})"    KDE4_FOUND)
    PRINT_LIBRARY_STATUS("libjpeg"   "http://www.ijg.org"                                               "(version >= 6b)"                    JPEG_FOUND)
    PRINT_LIBRARY_STATUS("libtiff"   "http://www.remotesensing.org/libtiff"                             "(version >= 3.8.2)"                 TIFF_FOUND)
    PRINT_LIBRARY_STATUS("libpng"    "http://www.libpng.org/pub/png/libpng.html"                        "(version >= 1.2.7)"                 PNG_FOUND)
    PRINT_LIBRARY_STATUS("libkipi"   "https://projects.kde.org/projects/kde/kdegraphics/libs/libkipi"   "(version >= ${KIPI_MIN_VERSION})"   KIPI_FOUND)
    PRINT_LIBRARY_STATUS("libkexiv2" "https://projects.kde.org/projects/kde/kdegraphics/libs/libkexiv2" "(version >= 1.2.0)"                 KEXIV2_FOUND)
    PRINT_LIBRARY_STATUS("libkdcraw" "https://projects.kde.org/projects/kde/kdegraphics/libs/libkdcraw" "(version >= ${KDCRAW_MIN_VERSION})" KDCRAW_FOUND)

    # Optional shared libraries results.

    PRINT_OPTIONAL_LIBRARY_STATUS("libxml2"        "http://xmlsoft.org"                                              "(version >= 2.7.0)"                  "Kipi-plugins will be compiled without libxml2 support."        LIBXML2_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libxslt"        "http://xmlsoft.org/XSLT"                                         "(version >= 1.1.0)"                  "Kipi-plugins will be compiled without libxstl support."        LIBXSLT_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libexpat"       "http://expat.sourceforge.net"                                    "(version >= 2.0.0)"                  "Kipi-plugins will be compiled without libexpat support."       EXPAT_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libopencv"      "http://opencv.willowgarage.com"                                  "(version >= ${OPENCV_MIN_VERSION})"  "Kipi-plugins will be compiled without libopencv support."      OpenCV_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libqjson"       "http://qjson.sourceforge.net"                                    "(version >= 0.6.0)"                  "Kipi-plugins will be compiled without libqjson support."       QJSON_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libkdepimlibs"  "http://pim.kde.org"                                              "(version >= 4.3.0)"                  "Kipi-plugins will be compiled without libkdepimlibs support."  KDEPIMLIBS_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libqca"         "http://delta.affinix.com/qca"                                    "(version >= 2.0.0"                   "Kipi-plugins will be compiled without libqca support."         QCA2_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libkgeomap"     "https://projects.kde.org/projects/extragear/libs/libkgeomap"     "(version >= ${KGEOMAP_MIN_VERSION})" "Kipi-plugins will be compiled without libkgeomap support."     KGEOMAP_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libmediawiki"   "https://projects.kde.org/projects/extragear/libs/libmediawiki"   "(version >= 1.0.0)"                  "Kipi-plugins will be compiled without libmediawiki support."   MEDIAWIKI_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libkvkontakte"  "https://projects.kde.org/projects/extragear/libs/libkvkontakte"  "(version >= 0.1.0)"                  "Kipi-plugins will be compiled without libkvkontakte support."  LIBKVKONTAKTE_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libimagemagick" "http://www.imagemagick.org"                                      "(version >= 5.5.4)"                  "Kipi-plugins will be compiled without libimagemagick support." ImageMagick_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libqtgstreamer" "http://gstreamer.freedesktop.org/modules/qt-gstreamer.html"      "(version >= 0.9.0)"                  "Kipi-plugins will be compiled without libqtgstreamer support." QTGSTREAMER_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libgpod"        "http://www.gtkpod.org/libgpod.html"                              "(version >= 0.7.0)"                  "Kipi-plugins will be compiled without libgpod support."        IPOD_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libgdk"         "http://library.gnome.org/devel/gdk-pixbuf"                       "(version >= 2.0.0)"                  "Kipi-plugins will be compiled without libgdk support."         GDK_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libqtkoauth"    "http://gitorious.org/kqoauth"                                    "(version >= 0.97)"                   "Kipi-plugins will be compiled without libqtkoauth support."    QTKOAUTH_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libksane"       "https://projects.kde.org/projects/kde/kdegraphics/libs/libksane" "(version >= 0.3.0)"                  "Kipi-plugins will be compiled without libksane support."       KSANE_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libhupnp"       "http://www.herqq.org"                                            "(version >= 2.0.0)"                  "Internal version used instead"                                 HUPNP_FOUND AND HUPNPAV_FOUND)
    PRINT_OPTIONAL_LIBRARY_STATUS("libqtsoap"      "https://github.com/commontk/QtSOAP"                              "(version >= 2.7.0)"                  "Kipi-plugins will be compiled without libqtsoap support."      QTSOAP_FOUND)

    PRINT_OPTIONAL_LIBRARY_STATUS("native threads support" "" "" "Kipi-plugins will be compiled without native threads support." CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
    PRINT_OPTIONAL_LIBRARY_STATUS("native Opengl support"  "" "" "Kipi-plugins will be compiled without native OpenGl support."  OPENGL_FOUND AND OPENGL_GLU_FOUND)
    PRINT_OPTIONAL_QTMODULE_STATUS("Qt4 OpenGL"            QT_QTOPENGL_FOUND)

    # LINUX Only.
    if(NOT WIN32 AND NOT APPLE)
        PRINT_OPTIONAL_LIBRARY_STATUS("libX11" "http://www.x11.org" "(version >= 1.1.5)" "Kipi-plugins will be compiled without libX11 support." X11_FOUND AND X11_Xrandr_FOUND)
    endif()

    # Optional plugins results ----------------------------------------------------------------------------

    message(STATUS "")

    if(QT_FOUND AND KDE4_FOUND AND KIPI_FOUND AND JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND)
        set(KIPI_COMPILE_MESSAGE "kipi-plugins will be compiled")
        FILL_WITH_DOTS(KIPI_COMPILE_MESSAGE)
        message(STATUS " ${KIPI_COMPILE_MESSAGE} YES")
        set(KIPIPLUGINS_CAN_BE_COMPILED true)
    else()
        message(FATAL_ERROR " kipi-plugins will not be compiled! Look README file for more details about dependencies...")
        set(KIPIPLUGINS_CAN_BE_COMPILED false)
    endif()

    PRINT_COMPONENT_COMPILE_STATUS("Shwup"       QCA2_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("YandexFotki" QCA2_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("HtmlExport"  LIBXML2_FOUND AND LIBXSLT_FOUND)

    # decide if AdvancedSlideshow and ImageViewer can be built
    if(NOT WIN32 AND NOT APPLE)
        if(X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_FOUND)
            set(BUILD_VIEWERS TRUE)
        else()
            set(BUILD_VIEWERS FALSE)
        endif()
    else()
        if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_FOUND AND QT_QTOPENGL_FOUND)
            set(BUILD_VIEWERS TRUE)
        elseif()
            set(BUILD_VIEWERS FALSE)
        endif()
    endif()

    # Decide if wallpaper plugin can be built
    # NOTE: See relevant patch which must be applied to KDE Plasma API :
    # https://git.reviewboard.kde.org/r/105319/
    # Relevant Bugzilla entry :
    # https://bugs.kde.org/show_bug.cgi?id=217950
    set(WALLPAPER_KDE_REQ_VER "5.0.0")

    if(KDE_VERSION)
        if(${KDE_VERSION} VERSION_LESS ${WALLPAPER_KDE_REQ_VER})
            set(BUILD_WALLPAPER FALSE)
        else()
            set(BUILD_WALLPAPER TRUE)
        endif()
    endif()

    PRINT_COMPONENT_COMPILE_STATUS("AdvancedSlideshow"  BUILD_VIEWERS)
    PRINT_COMPONENT_COMPILE_STATUS("ImageViewer"        BUILD_VIEWERS)
    PRINT_COMPONENT_COMPILE_STATUS("AcquireImages"      KSANE_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("DNGConverter"       EXPAT_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("DLNAExport"         (HUPNP_FOUND AND HUPNPAV_FOUND) OR (NOT (HUPNP_FOUND AND HUPNPAV_FOUND) AND QTSOAP_FOUND))
    PRINT_COMPONENT_COMPILE_STATUS("RemoveRedEyes"      OpenCV_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("Debian Screenshots" QJSON_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("Facebook"           QJSON_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("Imgur"              QJSON_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("GoogleDrive"        QJSON_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("Dropbox"            QJSON_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("VKontakte"          LIBKVKONTAKTE_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("IpodExport"         IPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("Calendar"           KDEPIMLIBS_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("GPSSync"            KGEOMAP_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("Mediawiki"          MEDIAWIKI_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("Panorama"           FLEX_FOUND AND BISON_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("VideoSlideshow"     ImageMagick_FOUND AND QTGSTREAMER_FOUND)
    PRINT_COMPONENT_COMPILE_STATUS("Wallpaper"          BUILD_WALLPAPER)


    # Set QtGStreamer API version --------------------------------------------------------------------------

    if(QTGSTREAMER_FOUND)

        message(STATUS "----------------------------------------------------------------------------------")
        message(STATUS "")

        if((${QtGStreamer_VERSION_MAJOR} EQUAL 1 AND ${QtGStreamer_VERSION_MINOR} EQUAL 1 AND ${QtGStreamer_VERSION_PATCH} GREATER 80)
           OR (${QtGStreamer_VERSION_MAJOR} EQUAL 1 AND ${QtGStreamer_VERSION_MINOR} GREATER 1) )
            set(GSTREAMER_API_1 TRUE)
        else()
            set(GSTREAMER_API_1 FALSE)
        endif()

        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/gstreamerapi.h.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/gstreamerapi.h" @ONLY)

        message(STATUS "Found QtGstreamer version: ${QtGStreamer_VERSION}")
        message(STATUS "Using GStreamer API 1:     ${GSTREAMER_API_1}")

        message(STATUS "----------------------------------------------------------------------------------")
        message(STATUS "")

    endif()

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
                            ${KEXIV2_INCLUDE_DIR}
                            ${KDCRAW_INCLUDE_DIR}
                            ${KIPI_INCLUDE_DIR}
                            ${KDE4_INCLUDES}
                            ${QT4_INCLUDES}
                        )

        add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

        # To prevent warnings from M$ compiler

        if(WIN32 AND MSVC)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            add_definitions(-D_ATL_SECURE_NO_WARNINGS)
            add_definitions(-D_AFX_SECURE_NO_WARNINGS)
        endif()

        # Debug settings

        set(AREA_CODE_GENERAL "51000")
        set(AREA_CODE_LOADING "51001")
        add_definitions(-DKDE_DEFAULT_DEBUG_AREA=${AREA_CODE_GENERAL})
        add_definitions(-DAREA_CODE_GENERAL=${AREA_CODE_GENERAL})
        add_definitions(-DAREA_CODE_LOADING=${AREA_CODE_LOADING})
        add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

        # kipi host test program to check plugins

        if(KDE4_BUILD_TESTS)
            add_subdirectory(tests)
        endif()

        # Plugins which will be always compiled

        add_subdirectory(common)
        add_subdirectory(timeadjust)
        add_subdirectory(jpeglossless)
        add_subdirectory(rawconverter)
        add_subdirectory(metadataedit)
        add_subdirectory(sendimages)
        add_subdirectory(flashexport)
        add_subdirectory(flickrexport)
        add_subdirectory(galleryexport)
        add_subdirectory(piwigoexport)
        add_subdirectory(kioexportimport)
        add_subdirectory(picasawebexport)
        add_subdirectory(smug)
        add_subdirectory(printimages)
        add_subdirectory(expoblending)
        add_subdirectory(kopete)
        add_subdirectory(batchprocessimages)
        add_subdirectory(rajceexport)
        add_subdirectory(kmlexport)
        add_subdirectory(imageshackexport)
        add_subdirectory(photolayoutseditor)
        add_subdirectory(jalbumexport)

        # Plugin not yet ready for production
        #add_subdirectory(photivointegration)

        # Plugins which are optional dependencies.

        if((HUPNP_FOUND AND HUPNPAV_FOUND) OR (NOT (HUPNP_FOUND AND HUPNPAV_FOUND) AND QTSOAP_FOUND))
            add_subdirectory(dlnaexport)
        endif()

        if(KDEPIMLIBS_FOUND)
            add_subdirectory(calendar)
        endif()

        if(QCA2_FOUND)
            add_subdirectory(shwup)
            add_subdirectory(yandexfotki)
        endif()

        if(IPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)
            add_subdirectory(ipodexport)
        endif()

        if(OpenCV_FOUND)
            add_subdirectory(removeredeyes)
        endif()

        if(QJSON_FOUND)
            add_subdirectory(debianscreenshots)
            add_subdirectory(facebook)
            add_subdirectory(imgurexport)
            add_subdirectory(googledrive)
            add_subdirectory(dropbox)
        endif()

        if(LIBKVKONTAKTE_FOUND)
            add_subdirectory(vkontakte)
        endif()

        # Currently the cmake module distributed w/ cmake uses a different variable
        # name LIBXLST_EXSLT_LIBRARIES for libxeslt than the cmake modeule 
        # distributed w/ kdelibs LIBEXSLT_LIBRARIES.
        if(NOT LIBXSLT_EXSLT_LIBRARIES AND LIBEXSLT_LIBRARIES)
            set(LIBXSLT_EXSLT_LIBRARIES ${LIBEXSLT_LIBRARIES})
        endif()

        if(LIBXML2_FOUND AND LIBXSLT_FOUND AND LIBXSLT_EXSLT_LIBRARIES)
            add_subdirectory(htmlexport)
        endif()

        if(KSANE_FOUND)
            add_subdirectory(acquireimages)
        endif()

        if(BUILD_VIEWERS)
            add_subdirectory(advancedslideshow)
            add_subdirectory(imageviewer)
        endif()

        if(EXPAT_FOUND)
            if(CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
                add_subdirectory(dngconverter)
            endif()
        endif()

        if(KGEOMAP_FOUND)
            add_subdirectory(gpssync)
        endif()

        if(MEDIAWIKI_FOUND)
            add_subdirectory(mediawiki)
        endif()

        if(FLEX_FOUND AND BISON_FOUND)
            add_subdirectory(panorama)
        endif()

        if(ImageMagick_FOUND AND QTGSTREAMER_FOUND)
            add_subdirectory(videoslideshow)
        endif()

        if(BUILD_WALLPAPER)
            add_subdirectory(wallpaper)
        endif()

        if(NOT WIN32)
            configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/kipi-plugins.lsm.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipi-plugins.lsm)
        endif()

    endif()

else()

    message(STATUS "")
    message(STATUS "----------------------------------------------------------------------------------")
    message(STATUS " libkipi not found. kipi-plugins ${KIPIPLUGINS_VERSION_STRING} will not be compiled")
    message(STATUS "")

endif()
