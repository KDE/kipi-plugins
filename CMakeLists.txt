PROJECT(kipi-plugins)

# =======================================================
# Information to update before to release this package.

# kipi-plugins version
SET(KIPIPLUGINS_MAJOR_VERSION "0")
SET(KIPIPLUGINS_MINOR_VERSION "8")
SET(KIPIPLUGINS_PATCH_VERSION "0")

# kipi-plugins release date
SET (KIPIPLUGINS_RELEASE_DATE "2009-10-xx")

# Suffix to add at end of version string. Usual values are:
# "-svn"   : alpha code unstable from svn. Do not use in production
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

# =======================================================

FIND_PACKAGE(KDE4 REQUIRED)

INCLUDE(MacroOptionalFindPackage)
INCLUDE(FindPackageHandleStandardArgs)
INCLUDE(KDE4Defaults)
INCLUDE(MacroLibrary)

# Depencies detection required by all plugins
FIND_PACKAGE(Kexiv2 REQUIRED)
FIND_PACKAGE(Kdcraw REQUIRED)
FIND_PACKAGE(Kipi REQUIRED)
FIND_PACKAGE(JPEG REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(TIFF REQUIRED)

# Optional depencies detection required by some plugins

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

MACRO_OPTIONAL_FIND_PACKAGE(EXPAT)      # For DNGConverter: XMP SDK need Expat library to compile.
MACRO_OPTIONAL_FIND_PACKAGE(Threads)    # For DNGConverter: DNG SDK need native threads support.
MACRO_OPTIONAL_FIND_PACKAGE(LibXml2)    # For Htmlexport.
MACRO_OPTIONAL_FIND_PACKAGE(LibXslt)    # For Htmlexport.
MACRO_OPTIONAL_FIND_PACKAGE(OpenGL)     # For AdvancedSlideshow and ImageViewer.
MACRO_OPTIONAL_FIND_PACKAGE(OpenCV)     # For RemoveRedEyes.
MACRO_OPTIONAL_FIND_PACKAGE(Gpod)       # For ipodexport.
MACRO_OPTIONAL_FIND_PACKAGE(Gdk)        # For ipodexport.
MACRO_OPTIONAL_FIND_PACKAGE(GLIB2)      # For ipodexport.
MACRO_OPTIONAL_FIND_PACKAGE(GObject)    # For ipodexport.
MACRO_OPTIONAL_FIND_PACKAGE(KdepimLibs) # For Calendar (libkcal).

# LINUX Only.
IF(NOT WIN32 AND NOT APPLE)
    MACRO_OPTIONAL_FIND_PACKAGE(Sane)  # For AcquireImages.
    MACRO_OPTIONAL_FIND_PACKAGE(X11)   # For AdvancedSlideshow and ImageViewer.
ENDIF(NOT WIN32 AND NOT APPLE)

IF(NOT WIN32)
    # Check KDcraw version installed to compile fine DngConverter plugin.
    FIND_PACKAGE(PkgConfig)
    PKG_CHECK_MODULES(Kdcraw libkdcraw>=0.4.0)
    IF(Kdcraw_FOUND)
        SET(KDCRAW_FOR_DNGCONVERTER TRUE)
    ELSE(Kdcraw_FOUND)
        SET(KDCRAW_FOR_DNGCONVERTER FALSE)
    ENDIF(Kdcraw_FOUND)
ELSE(NOT WIN32)
    SET(KDCRAW_FOR_DNGCONVERTER TRUE)
ENDIF(NOT WIN32)

INCLUDE(MacroOptionalDependPackage)

# LINUX Only.
IF(NOT WIN32 AND NOT APPLE)
    MACRO_OPTIONAL_DEPEND_PACKAGE(KSane "ksane")
        IF(DEPEND_PACKAGE_KSane)
        MACRO_OPTIONAL_FIND_PACKAGE(KSane)   # For AcquireImages
    ELSE(DEPEND_PACKAGE_KSane)
        SET(KSANE_FOUND FALSE)
    ENDIF(DEPEND_PACKAGE_KSane)
ENDIF(NOT WIN32 AND NOT APPLE)

# ==================================================================================================
# Log messages

MESSAGE(STATUS "")
MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS " kipi-plugins ${KIPIPLUGINS_VERSION_STRING} dependencies results   <http://www.kipi-plugins.org>")
MESSAGE(STATUS "")

# Require shared libraries results.

IF(JPEG_FOUND)
    MESSAGE(STATUS " libjpeg library found............... YES")
ELSE(JPEG_FOUND)
    MESSAGE(STATUS " libjpeg library found............... NO")
    MESSAGE(STATUS "")
    MESSAGE(SEND_ERROR " kipi-plugins needs libjpeg. You need to install the libjpeg development package.")
    MESSAGE(STATUS " libjpeg website is at http://www.ijg.org")
    MESSAGE(STATUS "")
ENDIF(JPEG_FOUND)

IF(TIFF_FOUND)
    MESSAGE(STATUS " libtiff library found............... YES")
ELSE(TIFF_FOUND)
    MESSAGE(STATUS " libtiff library found............... NO")
    MESSAGE(STATUS "")
    MESSAGE(SEND_ERROR " kipi-plugins needs libtiff. You need to install the libtiff development package.")
    MESSAGE(STATUS " libtiff website is at http://www.remotesensing.org/libtiff")
    MESSAGE(STATUS "")
ENDIF(TIFF_FOUND)

IF(PNG_FOUND)
    MESSAGE(STATUS " libpng library found................ YES")
ELSE(PNG_FOUND)
    MESSAGE(STATUS " libpng library found................ NO")
    MESSAGE(STATUS "")
    MESSAGE(SEND_ERROR " kipi-plugins needs libpng. You need to install the correct version (>= 1.2.7).")
    MESSAGE(STATUS " Libpng website is at http://www.libpng.org/pub/png/libpng.html")
    MESSAGE(STATUS "")
ENDIF(PNG_FOUND)

IF(KIPI_FOUND)
    MESSAGE(STATUS " libkipi library found............... YES")
ELSE(KIPI_FOUND)
    MESSAGE(STATUS " libkipi library found............... NO")
    MESSAGE(STATUS "")
    MESSAGE(SEND_ERROR " kipi-plugins needs libkipi library >= 0.2.0. You need to install libkipi first")
    MESSAGE(STATUS " libkipi website is at http://www.digikam.org/sharedlibs")
    MESSAGE(STATUS "")
ENDIF(KIPI_FOUND)

IF(KEXIV2_FOUND)
    MESSAGE(STATUS " libkexiv2 library found............. YES")
ELSE(KEXIV2_FOUND)
    MESSAGE(STATUS " libkexiv2 library found............. NO")
    MESSAGE(STATUS "")
    MESSAGE(SEND_ERROR " kipi-plugins needs libkexiv2 library >= 0.2.0. You need to install libkexiv2 first")
    MESSAGE(STATUS " libkexiv2 website is at http://www.digikam.org/sharedlibs")
    MESSAGE(STATUS "")
ENDIF(KEXIV2_FOUND)

IF(KDCRAW_FOUND)
    MESSAGE(STATUS " libkdcraw library found............. YES")
ELSE(KDCRAW_FOUND)
    MESSAGE(STATUS " libkdcraw library found............. NO")
    MESSAGE(STATUS "")
    MESSAGE(SEND_ERROR " kipi-plugins needs libkdcraw library >= 0.4.0. You need to install libkdcraw first")
    MESSAGE(STATUS " libkdcraw website is at http://www.digikam.org/sharedlibs")
    MESSAGE(STATUS "")
ENDIF(KDCRAW_FOUND)

# Optional shared libraries results.

IF(LIBXML2_FOUND)
    MESSAGE(STATUS " libxml2 library found............... YES (optional)")
ELSE(LIBXML2_FOUND)
    MESSAGE(STATUS " libxml2 library found............... NO  (optional)")
ENDIF(LIBXML2_FOUND)

IF(LIBXSLT_FOUND)
    MESSAGE(STATUS " libxslt library found............... YES (optional)")
ELSE(LIBXSLT_FOUND)
    MESSAGE(STATUS " libxslt library found............... NO  (optional)")
ENDIF(LIBXSLT_FOUND)

IF(EXPAT_FOUND)
    MESSAGE(STATUS " libexpat library found.............. YES (optional)")
ELSE(EXPAT_FOUND)
    MESSAGE(STATUS " libexpat library found.............. NO  (optional)")
ENDIF(EXPAT_FOUND)

IF(CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
    MESSAGE(STATUS " native threads support found........ YES (optional)")
ELSE(CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
    MESSAGE(STATUS " native threads support found........ NO  (optional)")
ENDIF(CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)

IF(OPENGL_FOUND AND OPENGL_GLU_FOUND)
    MESSAGE(STATUS " libopengl library found............. YES (optional)")
ELSE(OPENGL_FOUND AND OPENGL_GLU_FOUND)
    MESSAGE(STATUS " libopengl library found............. NO  (optional)")
ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND)

IF(OPENCV_FOUND)
    MESSAGE(STATUS " libopencv library found............. YES (optional)")
ELSE(OPENCV_FOUND)
    MESSAGE(STATUS " libopencv library found............. NO  (optional)")
ENDIF(OPENCV_FOUND)

IF(GPOD_FOUND)
    MESSAGE(STATUS " libgpod library found............... YES (optional)")
ELSE(GPOD_FOUND)
    MESSAGE(STATUS " libgpod library found............... NO  (optional)")
ENDIF(GPOD_FOUND)

IF(GDK_FOUND)
    MESSAGE(STATUS " Gdk library found................... YES (optional)")
ELSE(GDK_FOUND)
    MESSAGE(STATUS " Gdk library found................... NO  (optional)")
ENDIF(GDK_FOUND)

IF(KDEPIMLIBS_FOUND)
    MESSAGE(STATUS " libkdepim library found............. YES (optional)")
ELSE(KDEPIMLIBS_FOUND)
    MESSAGE(STATUS " libkdepim library found............. NO  (optional)")
ENDIF(KDEPIMLIBS_FOUND)

# LINUX Only.
IF(NOT WIN32 AND NOT APPLE)
    IF(X11_FOUND AND X11_Xrandr_FOUND)
        MESSAGE(STATUS " libX11 library found................ YES (optional)")
    ELSE(X11_FOUND AND X11_Xrandr_FOUND)
        MESSAGE(STATUS " libX11 library found................ NO  (optional)")
    ENDIF(X11_FOUND AND X11_Xrandr_FOUND)
ENDIF(NOT WIN32 AND NOT APPLE)

IF(NOT APPLE)
    IF(KSANE_FOUND)
        MESSAGE(STATUS " libksane library found.............. YES (optional)")
    ELSE(KSANE_FOUND)
        MESSAGE(STATUS " libksane library found.............. NO  (optional)")
    ENDIF(KSANE_FOUND)
ENDIF(NOT APPLE)

# Optional plugins results ----------------------------------------------------------------------------

MESSAGE(STATUS "")

IF(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)
    MESSAGE(STATUS " kipi-plugins will be compiled....... YES")
    SET(KIPIPLUGINS_CAN_BE_COMPILED true)
ELSE(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)
    MESSAGE(FATAL_ERROR " kipi-plugins will not be compiled! Look README file for more details about dependencies...")
    SET(KIPIPLUGINS_CAN_BE_COMPILED false)
ENDIF(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)

IF(LIBXML2_FOUND AND LIBXSLT_FOUND)
    MESSAGE(STATUS " HtmlExport will be compiled......... YES (optional)")
ELSE(LIBXML2_FOUND AND LIBXSLT_FOUND)
    MESSAGE(STATUS " HtmlExport will be compiled......... NO  (optional - Look README file for more details about dependencies)")
ENDIF(LIBXML2_FOUND AND LIBXSLT_FOUND)

IF(NOT WIN32 AND NOT APPLE)

    IF(X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND)
        MESSAGE(STATUS " AdvancedSlideshow will be compiled.. YES (optional)")
        MESSAGE(STATUS " ImageViewer will be compiled........ YES (optional)")
    ELSE(X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND)
        MESSAGE(STATUS " AdvancedSlideshow will be compiled.. NO  (optional - Look README file for more details about dependencies)")
        MESSAGE(STATUS " ImageViewer will be compiled........ NO  (optional - Look README file for more details about dependencies)")
    ENDIF(X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND)

ELSE(NOT WIN32 AND NOT APPLE)

    IF(OPENGL_FOUND AND OPENGL_GLU_FOUND)
        MESSAGE(STATUS " AdvancedSlideshow will be compiled.. YES (optional)")
        MESSAGE(STATUS " ImageViewer will be compiled........ YES (optional)")
    ELSE(OPENGL_FOUND AND OPENGL_GLU_FOUND)
        MESSAGE(STATUS " AdvancedSlideshow will be compiled.. NO  (optional - Look README file for more details about dependencies)")
        MESSAGE(STATUS " ImageViewer will be compiled........ NO  (optional - Look README file for more details about dependencies)")
    ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND)

ENDIF(NOT WIN32 AND NOT APPLE)

IF(NOT APPLE)

    IF(SANE_FOUND AND KSANE_FOUND)
        MESSAGE(STATUS " AcquireImages will be compiled...... YES (optional)")
    ELSE(SANE_FOUND AND KSANE_FOUND)
        MESSAGE(STATUS " AcquireImages will be compiled...... NO  (optional - Look README file for more details about dependencies)")
    ENDIF(SANE_FOUND AND KSANE_FOUND)

ENDIF(NOT APPLE)

IF(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)
    MESSAGE(STATUS " DNGConverter will be compiled....... YES (optional)")
ELSE(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)
    MESSAGE(STATUS " DNGConverter will be compiled....... NO  (optional - Look README file for more details about dependencies)")
ENDIF(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)

IF(OPENCV_FOUND)
    MESSAGE(STATUS " RemoveRedEyes will be compiled...... YES (optional)")
ELSE(OPENCV_FOUND)
    MESSAGE(STATUS " RemoveRedEyes will be compiled...... NO  (optional - Look README file for more details about dependencies)")
ENDIF(OPENCV_FOUND)

IF(GPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)
    MESSAGE(STATUS " IpodExport will be compiled......... YES (optional)")
ELSE(GPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)
    MESSAGE(STATUS " IpodExport will be compiled......... NO  (optional - Look README file for more details about dependencies)")
ENDIF(GPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)

IF(KDEPIMLIBS_FOUND)
    MESSAGE(STATUS " Calendar will be compiled........... YES (optional)")
ELSE(KDEPIMLIBS_FOUND)
    MESSAGE(STATUS " Calendar will be compiled........... NO  (optional - Look README file for more details about dependencies)")
ENDIF(KDEPIMLIBS_FOUND)

MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS "")

# ==================================================================================================

IF(KIPIPLUGINS_CAN_BE_COMPILED)

    # ==================================================================================================
    # Create svn version header

    # See http://public.kitware.com/pipermail/cmake/2006-July/010299.html
    # We only do this IF we are in a .svn dir

    FIND_FILE(SVN_MARKER entries PATHS ${CMAKE_SOURCE_DIR}/.svn)

    IF(NOT SVN_MARKER)
        SET (SVN_MARKER ${CMAKE_SOURCE_DIR}/CMakeLists.txt)  # Dummy file
    ENDIF(NOT SVN_MARKER)

    # Add a custom command to drive the svn script whenever the svn entries
    # file changes.
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/svnscript.cmake.in_cmake"
                   "${CMAKE_CURRENT_BINARY_DIR}/svnscript.cmake"
                   @ONLY)

    ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins/svnversion.h"
                       DEPENDS ${SVN_MARKER}
                       COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/svnscript.cmake")

    # Add a custom target to drive the custom command.
    ADD_CUSTOM_TARGET(kipiplugins-svnversion
                      ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins/svnversion.h")

    # ==================================================================================================

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins
                        ${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins
                        ${KEXIV2_INCLUDE_DIR}
                        ${KDCRAW_INCLUDE_DIR}
                        ${KIPI_INCLUDE_DIR}
                        ${KDE4_INCLUDES}
                        ${QT4_INCLUDES}
                       )

    # Plugins which will be always compiled

    ADD_SUBDIRECTORY(common)
    ADD_SUBDIRECTORY(timeadjust)
    ADD_SUBDIRECTORY(jpeglossless)
    ADD_SUBDIRECTORY(rawconverter)
    ADD_SUBDIRECTORY(metadataedit)
    ADD_SUBDIRECTORY(sendimages)
    ADD_SUBDIRECTORY(gpssync)
    ADD_SUBDIRECTORY(flashexport)
    ADD_SUBDIRECTORY(flickrexport)
    ADD_SUBDIRECTORY(galleryexport)
    ADD_SUBDIRECTORY(kioexportimport)
    ADD_SUBDIRECTORY(picasawebexport)
    ADD_SUBDIRECTORY(smug)
    ADD_SUBDIRECTORY(facebook)
    ADD_SUBDIRECTORY(printimages)

    # Plugins which are optional depencies.

    IF(KDEPIMLIBS_FOUND)
       ADD_SUBDIRECTORY(calendar)
    ENDIF(KDEPIMLIBS_FOUND)

    IF(GPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)
       ADD_SUBDIRECTORY(ipodexport)
    ENDIF(GPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)

    IF(OPENCV_FOUND)
        ADD_SUBDIRECTORY(removeredeyes)
    ENDIF(OPENCV_FOUND)

    IF(LIBXML2_FOUND AND LIBXSLT_FOUND)
        ADD_SUBDIRECTORY(htmlexport)
    ENDIF(LIBXML2_FOUND AND LIBXSLT_FOUND)

    IF(NOT WIN32 AND NOT APPLE)

        # Plugins which depend on OpenGL
        IF(X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND)
            # Under LINUX we need X11 library with OpenGL.
            ADD_SUBDIRECTORY(advancedslideshow)
            ADD_SUBDIRECTORY(imageviewer)
        ENDIF(X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND)

        # Under Linux, this plugin use KSane interface
        IF(SANE_FOUND AND KSANE_FOUND)
            ADD_SUBDIRECTORY(acquireimages)
        ENDIF(SANE_FOUND AND KSANE_FOUND)

    ELSE(NOT WIN32 AND NOT APPLE)

        # Plugins which depend on OpenGL
        IF(OPENGL_FOUND AND OPENGL_GLU_FOUND)
            ADD_SUBDIRECTORY(advancedslideshow)
            ADD_SUBDIRECTORY(imageviewer)
        ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND)

        # Under Windows, this plugin use TWAIN interface
        ADD_SUBDIRECTORY(acquireimages)

    ENDIF(NOT WIN32 AND NOT APPLE)

    IF(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)
        IF(CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
            ADD_SUBDIRECTORY(dngconverter)
        ENDIF(CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
    ENDIF(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)

#   Laurent Montel : wallpaper is currently disabled. Waiting new api with KDE 4.2 
#
#   FIND_FILE(KDESKTOP_INTERFACE_FILE_FOUND org.kde.kdesktop.Background.xml PATHS ${DBUS_INTERFACES_INSTALL_DIR} )
#
#   IF(KDESKTOP_INTERFACE_FILE_FOUND)
#       ADD_SUBDIRECTORY( wallpaper )
#   ELSE(KDESKTOP_INTERFACE_FILE_FOUND)
#       MESSAGE(STATUS "Wallpaper plugins will not compile. Kdebase (kdesktop) must be installed before compiling it")
#   ENDIF(KDESKTOP_INTERFACE_FILE_FOUND)

    ADD_SUBDIRECTORY(batchprocessimages)

    IF(NOT WIN32)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/kipi-plugins.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipi-plugins.lsm)
    ENDIF(NOT WIN32)

ENDIF(KIPIPLUGINS_CAN_BE_COMPILED)
