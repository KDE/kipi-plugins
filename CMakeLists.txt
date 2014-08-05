PROJECT(kipi-plugins)

MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS "Starting CMake configuration for: kipi-plugins")

# =======================================================
# Information to update before to release this package.

# kipi-plugins version
SET(KIPIPLUGINS_MAJOR_VERSION "4")
SET(KIPIPLUGINS_MINOR_VERSION "3")
SET(KIPIPLUGINS_PATCH_VERSION "0")

# kipi-plugins release date
SET (KIPIPLUGINS_RELEASE_DATE "2014-09-07")

# Suffix to add at end of version string. Usual values are:
# "-git"   : alpha code unstable from git. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
SET(KIPIPLUGINS_SUFFIX_VERSION "")

# =======================================================
# Set env. variables accordinly.

SET(KIPIPLUGINS_VERSION_STRING
    "${KIPIPLUGINS_MAJOR_VERSION}.${KIPIPLUGINS_MINOR_VERSION}.${KIPIPLUGINS_PATCH_VERSION}${KIPIPLUGINS_SUFFIX_VERSION}"
   )
   
# NOTE: This string is used to set libkipiplugins SO version ID
SET(KIPIPLUGINS_LIB_SO_VERSION_STRING
    "${KIPIPLUGINS_MAJOR_VERSION}.${KIPIPLUGINS_MINOR_VERSION}.${KIPIPLUGINS_PATCH_VERSION}"
   )

# =======================================================

CMAKE_POLICY(SET CMP0017 NEW)

FIND_PACKAGE(KDE4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

INCLUDE(MacroOptionalFindPackage)
INCLUDE(FindPackageHandleStandardArgs)
INCLUDE(KDE4Defaults)
INCLUDE(MacroLibrary)

# Put local modules first
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

# Dependencies detection required by all plugins

SET(KIPI_MIN_VERSION "2.0.0")

FIND_PACKAGE(Kipi ${KIPI_MIN_VERSION} REQUIRED)
FIND_PACKAGE(Kexiv2                   REQUIRED)
FIND_PACKAGE(JPEG                     REQUIRED)
FIND_PACKAGE(PNG                      REQUIRED)
FIND_PACKAGE(TIFF                     REQUIRED)
FIND_PACKAGE(Kdcraw                   REQUIRED)

# Check the libkdcraw version

SET(KDCRAW_MIN_VERSION "2.2.0")

IF (KDCRAW_VERSION)

    MESSAGE(STATUS "libkdcraw: Found version ${KDCRAW_VERSION} (required: ${KDCRAW_MIN_VERSION})")
    IF (${KDCRAW_VERSION} VERSION_LESS ${KDCRAW_MIN_VERSION})
        SET(KDCRAW_FOUND FALSE)
    ENDIF (${KDCRAW_VERSION} VERSION_LESS ${KDCRAW_MIN_VERSION})

ELSE (KDCRAW_VERSION)

    MESSAGE(WARNING "libkdcraw: Version information not found, your version is probably too old.")
    SET(KDCRAW_FOUND FALSE)

ENDIF (KDCRAW_VERSION)

# Optional dependencies detection required by some plugins
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
MACRO_OPTIONAL_FIND_PACKAGE(ImageMagick COMPONENTS MagickCore display)   # For VideoSlideshow.
MACRO_OPTIONAL_FIND_PACKAGE(QtGStreamer)                                 # For VideoSlideshow.
MACRO_OPTIONAL_FIND_PACKAGE(QtKOAuth)                                    # For Imgur Auhtenticated API

# Check the libkgeomap version
# note that old versions don't specify version information, so for now
# just inform the user if version information was found
IF (KGEOMAP_VERSION)
    MESSAGE(STATUS "libkgeomap: Found version ${KGEOMAP_VERSION}")
    # TODO: fail if the version is too low
ENDIF (KGEOMAP_VERSION)

# X11 check, LINUX Only.
IF(NOT WIN32 AND NOT APPLE)
    MACRO_OPTIONAL_FIND_PACKAGE(X11)                                     # For AdvancedSlideshow and ImageViewer.
ENDIF(NOT WIN32 AND NOT APPLE)

INCLUDE(MacroUtils)
INCLUDE(MacroOpenCV)
DETECT_OPENCV(2.4.9 core highgui objdetect contrib legacy imgproc)

INCLUDE(MacroOptionalDependPackage)

# ==================================================================================================
# Log messages

MESSAGE(STATUS "")
MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS " kipi-plugins ${KIPIPLUGINS_VERSION_STRING} dependencies results   <http://www.digikam.org>")
MESSAGE(STATUS "")

# Require shared libraries results.

PRINT_LIBRARY_STATUS("libjpeg"   "http://www.ijg.org"                        ""                                    JPEG_FOUND)
PRINT_LIBRARY_STATUS("libtiff"   "http://www.remotesensing.org/libtiff"      ""                                    TIFF_FOUND)
PRINT_LIBRARY_STATUS("libpng"    "http://www.libpng.org/pub/png/libpng.html" " (version >= 1.2.7)"                 PNG_FOUND)
PRINT_LIBRARY_STATUS("libkipi"   "http://www.digikam.org/sharedlibs"         " (version >= ${KIPI_MIN_VERSION})"   KIPI_FOUND)
PRINT_LIBRARY_STATUS("libkexiv2" "http://www.digikam.org/sharedlibs"         " (version >= 1.2.0)"                 KEXIV2_FOUND)
PRINT_LIBRARY_STATUS("libkdcraw" "http://www.digikam.org/sharedlibs"         " (version >= ${KDCRAW_MIN_VERSION})" KDCRAW_FOUND)

# Optional shared libraries results.

PRINT_OPTIONAL_LIBRARY_STATUS("libxml2"                LIBXML2_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libxslt"                LIBXSLT_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libexpat"               EXPAT_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("native threads support" CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
PRINT_OPTIONAL_LIBRARY_STATUS("libopengl"              OPENGL_FOUND AND OPENGL_GLU_FOUND)
PRINT_OPTIONAL_QTMODULE_STATUS("Qt4 OpenGL"            QT_QTOPENGL_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libopencv"              OpenCV_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("QJson"                  QJSON_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libgpod"                IPOD_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("Gdk"                    GDK_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkdepim"              KDEPIMLIBS_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("qca2"                   QCA2_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkgeomap"             KGEOMAP_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libmediawiki"           MEDIAWIKI_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkvkontakte"          LIBKVKONTAKTE_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libimagemagick"         ImageMagick_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libqtgstreamer"         QTGSTREAMER_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("QtKOAuth"               QTKOAUTH_FOUND)

# LINUX Only.
IF(NOT WIN32 AND NOT APPLE)
    PRINT_OPTIONAL_LIBRARY_STATUS("libX11" X11_FOUND AND X11_Xrandr_FOUND)
ENDIF(NOT WIN32 AND NOT APPLE)

PRINT_OPTIONAL_LIBRARY_STATUS("libksane" KSANE_FOUND)

IF(HUPNP_FOUND)
    MESSAGE(STATUS " Hupnp library found...................... YES (optional)")
ELSE(HUPNP_FOUND)
    MESSAGE(STATUS " Hupnp library found...................... NO  (optional - internal version used instead)")
ENDIF(HUPNP_FOUND)

# Optional plugins results ----------------------------------------------------------------------------

MESSAGE(STATUS "")

IF(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)
    SET(KIPI_COMPILE_MESSAGE "kipi-plugins will be compiled")
    FILL_WITH_DOTS(KIPI_COMPILE_MESSAGE)
    MESSAGE(STATUS " ${KIPI_COMPILE_MESSAGE} YES")
    SET(KIPIPLUGINS_CAN_BE_COMPILED true)
ELSE(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)
    MESSAGE(FATAL_ERROR " kipi-plugins will not be compiled! Look README file for more details about dependencies...")
    SET(KIPIPLUGINS_CAN_BE_COMPILED false)
ENDIF(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)

PRINT_COMPONENT_COMPILE_STATUS("Shwup" QCA2_FOUND)
PRINT_COMPONENT_COMPILE_STATUS("YandexFotki" QCA2_FOUND)
PRINT_COMPONENT_COMPILE_STATUS("HtmlExport" LIBXML2_FOUND AND LIBXSLT_FOUND)

# decide if AdvancedSlideshow and ImageViewer can be built
IF(NOT WIN32 AND NOT APPLE)
    IF (X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_FOUND)
        SET(BUILD_VIEWERS TRUE)
    ELSE (X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_FOUND)
        SET(BUILD_VIEWERS FALSE)
    ENDIF(X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_FOUND)
ELSE(NOT WIN32 AND NOT APPLE)
    IF (OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_FOUND AND QT_QTOPENGL_FOUND)
        SET(BUILD_VIEWERS TRUE)
    ELSEIF (OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_FOUND AND QT_QTOPENGL_FOUND)
        SET(BUILD_VIEWERS FALSE)
    ENDIF (OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_FOUND AND QT_QTOPENGL_FOUND)
ENDIF(NOT WIN32 AND NOT APPLE)

# decide if wallpaper plugin can be built
# NOTE: See relevant patch which must be applied to KDE Plasma API :
# https://git.reviewboard.kde.org/r/105319/
# Relevant Bugzilla entry :
# https://bugs.kde.org/show_bug.cgi?id=217950
SET (WALLPAPER_KDE_REQ_VER "5.0.0")

IF (KDE_VERSION)
    IF (${KDE_VERSION} VERSION_LESS ${WALLPAPER_KDE_REQ_VER})
        SET(BUILD_WALLPAPER FALSE)
    ELSE (${KDE_VERSION} VERSION_LESS ${WALLPAPER_KDE_REQ_VER})
        SET(BUILD_WALLPAPER TRUE)
    ENDIF (${KDE_VERSION} VERSION_LESS ${WALLPAPER_KDE_REQ_VER})
ENDIF (KDE_VERSION)

PRINT_COMPONENT_COMPILE_STATUS("AdvancedSlideshow"  BUILD_VIEWERS)
PRINT_COMPONENT_COMPILE_STATUS("ImageViewer"        BUILD_VIEWERS)
PRINT_COMPONENT_COMPILE_STATUS("AcquireImages"      KSANE_FOUND)
PRINT_COMPONENT_COMPILE_STATUS("DNGConverter"       EXPAT_FOUND)
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

IF (QTGSTREAMER_FOUND)

    MESSAGE(STATUS "----------------------------------------------------------------------------------")
    MESSAGE(STATUS "")

    IF( (${QtGStreamer_VERSION_MAJOR} EQUAL 1 AND ${QtGStreamer_VERSION_MINOR} EQUAL 1 AND ${QtGStreamer_VERSION_PATCH} GREATER 80)
        OR (${QtGStreamer_VERSION_MAJOR} EQUAL 1 AND ${QtGStreamer_VERSION_MINOR} GREATER 1) )
        SET(GSTREAMER_API_1 TRUE)
    ELSE()
        SET(GSTREAMER_API_1 FALSE)
    ENDIF()

    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/gstreamerapi.h.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/gstreamerapi.h" @ONLY)

    MESSAGE(STATUS "Found QtGstreamer version : ${QtGStreamer_VERSION}")
    MESSAGE(STATUS "Using GStreamer API 1: ${GSTREAMER_API_1}")


    MESSAGE(STATUS "----------------------------------------------------------------------------------")
    MESSAGE(STATUS "")

ENDIF()
    
# ==================================================================================================

IF(KIPIPLUGINS_CAN_BE_COMPILED)

    # ==================================================================================================
    # Create git version header

    # We only do this IF we are in a .git dir
    FIND_FILE(GIT_MARKER entries PATHS ${CMAKE_SOURCE_DIR}/.git)

    IF(NOT GIT_MARKER)
        SET (GIT_MARKER ${CMAKE_SOURCE_DIR}/CMakeLists.txt)  # Dummy file
    ENDIF(NOT GIT_MARKER)

    # Add a custom command to drive the git script whenever the git entries
    # file changes.
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/gitscript.cmake.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/gitscript.cmake"
                   @ONLY)

    # Add a custom target to drive the custom command.
    ADD_CUSTOM_TARGET(kipiplugins-gitversion
                      ALL COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/gitscript.cmake")

    # ==================================================================================================

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins
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

    # To prevent warnings from M$ compiler

    IF(WIN32 AND MSVC)
        ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
        ADD_DEFINITIONS(-D_ATL_SECURE_NO_WARNINGS)
        ADD_DEFINITIONS(-D_AFX_SECURE_NO_WARNINGS)
    ENDIF(WIN32 AND MSVC)

    # Debug settings

    SET(AREA_CODE_GENERAL "51000")
    SET(AREA_CODE_LOADING "51001")
    ADD_DEFINITIONS(-DKDE_DEFAULT_DEBUG_AREA=${AREA_CODE_GENERAL})
    ADD_DEFINITIONS(-DAREA_CODE_GENERAL=${AREA_CODE_GENERAL})
    ADD_DEFINITIONS(-DAREA_CODE_LOADING=${AREA_CODE_LOADING})
    ADD_DEFINITIONS(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

    # kipi host test program to check plugins

    if(KDE4_BUILD_TESTS)
        ADD_SUBDIRECTORY(tests)
    ENDIF(KDE4_BUILD_TESTS)

    # Plugins which will be always compiled

    ADD_SUBDIRECTORY(common)
    ADD_SUBDIRECTORY(timeadjust)
    ADD_SUBDIRECTORY(jpeglossless)
    ADD_SUBDIRECTORY(rawconverter)
    ADD_SUBDIRECTORY(metadataedit)
    ADD_SUBDIRECTORY(sendimages)
    ADD_SUBDIRECTORY(flashexport)
    ADD_SUBDIRECTORY(flickrexport)
    ADD_SUBDIRECTORY(galleryexport)
    ADD_SUBDIRECTORY(piwigoexport)
    ADD_SUBDIRECTORY(kioexportimport)
    ADD_SUBDIRECTORY(picasawebexport)
    ADD_SUBDIRECTORY(smug)
    ADD_SUBDIRECTORY(printimages)
    ADD_SUBDIRECTORY(expoblending)
    ADD_SUBDIRECTORY(kopete)
    ADD_SUBDIRECTORY(batchprocessimages)
    ADD_SUBDIRECTORY(rajceexport)
    ADD_SUBDIRECTORY(kmlexport)
    ADD_SUBDIRECTORY(imageshackexport)
    ADD_SUBDIRECTORY(photolayoutseditor)
    ADD_SUBDIRECTORY(dlnaexport)
    ADD_SUBDIRECTORY(jalbumexport)

    # Plugin not yet ready for production
    #ADD_SUBDIRECTORY(photivointegration)

    # Plugins which are optional dependencies.

    IF(KDEPIMLIBS_FOUND)
       ADD_SUBDIRECTORY(calendar)
    ENDIF(KDEPIMLIBS_FOUND)

    IF(QCA2_FOUND)
       ADD_SUBDIRECTORY(shwup)
       ADD_SUBDIRECTORY(yandexfotki)
    ENDIF(QCA2_FOUND)

    IF(IPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)
       ADD_SUBDIRECTORY(ipodexport)
    ENDIF(IPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)

    IF(OpenCV_FOUND)
        ADD_SUBDIRECTORY(removeredeyes)
    ENDIF(OpenCV_FOUND)

    IF(QJSON_FOUND)
        ADD_SUBDIRECTORY(debianscreenshots)
        ADD_SUBDIRECTORY(facebook)
        ADD_SUBDIRECTORY(imgurexport)
        ADD_SUBDIRECTORY(googledrive)
        ADD_SUBDIRECTORY(dropbox)
    ENDIF(QJSON_FOUND)

    IF(LIBKVKONTAKTE_FOUND)
        ADD_SUBDIRECTORY(vkontakte)
    ENDIF(LIBKVKONTAKTE_FOUND)

    # Currently the cmake module distributed w/ cmake uses a different variable
    # name LIBXLST_EXSLT_LIBRARIES for libxeslt than the cmake modeule 
    # distributed w/ kdelibs LIBEXSLT_LIBRARIES.
    IF (NOT LIBXSLT_EXSLT_LIBRARIES AND LIBEXSLT_LIBRARIES)
        SET(LIBXSLT_EXSLT_LIBRARIES ${LIBEXSLT_LIBRARIES})
    ENDIF (NOT LIBXSLT_EXSLT_LIBRARIES AND LIBEXSLT_LIBRARIES)
    IF(LIBXML2_FOUND AND LIBXSLT_FOUND AND LIBXSLT_EXSLT_LIBRARIES)
        ADD_SUBDIRECTORY(htmlexport)
    ENDIF(LIBXML2_FOUND AND LIBXSLT_FOUND AND LIBXSLT_EXSLT_LIBRARIES)

    IF(KSANE_FOUND)
        ADD_SUBDIRECTORY(acquireimages)
    ENDIF(KSANE_FOUND)

    IF(BUILD_VIEWERS)
        ADD_SUBDIRECTORY(advancedslideshow)
        ADD_SUBDIRECTORY(imageviewer)
    ENDIF(BUILD_VIEWERS)

    IF(EXPAT_FOUND)
        IF(CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
            ADD_SUBDIRECTORY(dngconverter)
        ENDIF(CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
    ENDIF(EXPAT_FOUND)

    IF(KGEOMAP_FOUND)
        ADD_SUBDIRECTORY(gpssync)
    ENDIF(KGEOMAP_FOUND)

    IF(MEDIAWIKI_FOUND)
        ADD_SUBDIRECTORY(mediawiki)
    ENDIF(MEDIAWIKI_FOUND)

    IF(FLEX_FOUND AND BISON_FOUND)
        ADD_SUBDIRECTORY(panorama)
    ENDIF(FLEX_FOUND AND BISON_FOUND)

    IF(ImageMagick_FOUND AND QTGSTREAMER_FOUND)
        ADD_SUBDIRECTORY(videoslideshow)
    ENDIF(ImageMagick_FOUND AND QTGSTREAMER_FOUND)

    IF(BUILD_WALLPAPER)
        ADD_SUBDIRECTORY(wallpaper)
    ENDIF(BUILD_WALLPAPER)

    IF(NOT WIN32)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/kipi-plugins.lsm.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipi-plugins.lsm)
    ENDIF(NOT WIN32)

ENDIF(KIPIPLUGINS_CAN_BE_COMPILED)
