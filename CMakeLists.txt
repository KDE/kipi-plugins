PROJECT(kipi-plugins)

# =======================================================
# Information to update before to release this package.

# kipi-plugins version
SET(KIPIPLUGINS_MAJOR_VERSION "1")
SET(KIPIPLUGINS_MINOR_VERSION "3")
SET(KIPIPLUGINS_PATCH_VERSION "0")

# kipi-plugins release date
SET (KIPIPLUGINS_RELEASE_DATE "2010-05-30")

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
# some useful macros for printing status information

SET(PRINT_COMPILE_LENGTH "40")
MACRO(FILL_WITH_DOTS VAR)

    STRING(LENGTH ${${VAR}} NAME_LENGTH)

    MATH(EXPR DOT_LENGTH "${PRINT_COMPILE_LENGTH} - ${NAME_LENGTH}")
    IF(${DOT_LENGTH} LESS 0)
        SET(DOT_LENGTH 0)
    ENDIF(${DOT_LENGTH} LESS 0)

    FOREACH(COUNT RANGE ${DOT_LENGTH})
        SET(${VAR} "${${VAR}}.")
    ENDFOREACH(COUNT)

ENDMACRO(FILL_WITH_DOTS)

MACRO(PRINT_PLUGIN_COMPILE_STATUS NAME)

    SET(COMPILE_MESSAGE "${NAME} will be compiled")
    FILL_WITH_DOTS(COMPILE_MESSAGE)

    IF(${ARGN})
        MESSAGE(STATUS " ${COMPILE_MESSAGE} YES (optional)")
    ELSE(${ARGN})
        MESSAGE(STATUS " ${COMPILE_MESSAGE} NO  (optional - Look README file for more details about dependencies)")
    ENDIF(${ARGN})

ENDMACRO(PRINT_PLUGIN_COMPILE_STATUS)

MACRO(PRINT_OPTIONAL_LIBRARY_STATUS NAME)

    SET(LIB_MESSAGE "${NAME} library found")
    FILL_WITH_DOTS(LIB_MESSAGE)

    IF(${ARGN})
        MESSAGE(STATUS " ${LIB_MESSAGE} YES (optional)")
    ELSE(${ARGN})
        MESSAGE(STATUS " ${LIB_MESSAGE} NO  (optional)")
    ENDIF(${ARGN})

ENDMACRO(PRINT_OPTIONAL_LIBRARY_STATUS)

MACRO(PRINT_LIBRARY_STATUS NAME WEBSITE VERSIONHINT)

    SET(LIB_MESSAGE "${NAME} library found")
    FILL_WITH_DOTS(LIB_MESSAGE)

    IF(${ARGN})
        MESSAGE(STATUS " ${LIB_MESSAGE} YES")
    ELSE(${ARGN})
        MESSAGE(STATUS " ${LIB_MESSAGE} NO")
        MESSAGE(STATUS "")
        MESSAGE(SEND_ERROR " kipi-plugins needs ${NAME}. You need to install the ${NAME}${VERSIONHINT} library development package.")
        MESSAGE(STATUS " ${NAME} website is at ${WEBSITE}")
        MESSAGE(STATUS "")
    ENDIF(${ARGN})

ENDMACRO(PRINT_LIBRARY_STATUS)

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
MACRO_OPTIONAL_FIND_PACKAGE(QCA2)       # For Shwup.
MACRO_OPTIONAL_FIND_PACKAGE(KSane)      # For AcquireImages.

# X11 check, LINUX Only.
IF(NOT WIN32 AND NOT APPLE)
    MACRO_OPTIONAL_FIND_PACKAGE(X11)   # For AdvancedSlideshow and ImageViewer.
ENDIF(NOT WIN32 AND NOT APPLE)

IF(NOT WIN32)
    FIND_PACKAGE(PkgConfig)
    # Check KDcraw version.
    PKG_CHECK_MODULES(Kdcraw libkdcraw>=1.2.0)
    IF(Kdcraw_FOUND)
        SET(KDCRAW_FOR_KIPIPLUGINS TRUE)
    ELSE(Kdcraw_FOUND)
        SET(KDCRAW_FOR_KIPIPLUGINS FALSE)
    ENDIF(Kdcraw_FOUND)
ELSEIF(NOT WIN32)
    SET(KDCRAW_FOR_KIPIPLUGINS TRUE)
ENDIF(NOT WIN32)

# OpenMP detection requires CMake >= 2.6.3

EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} --version OUTPUT_VARIABLE VERSION_CMAKE_INFO)
MESSAGE(STATUS "CMake version: ${VERSION_CMAKE_INFO}")
STRING(REPLACE "-patch " "." VERSION_CMAKE_INFO "${VERSION_CMAKE_INFO}")
MESSAGE(STATUS "CMake version (cleaned): ${VERSION_CMAKE_INFO}")
MACRO_ENSURE_VERSION("2.6.3" "${VERSION_CMAKE_INFO}" OPENMP_VERSION_CMAKE)

IF(OPENMP_VERSION_CMAKE)
    MACRO_OPTIONAL_FIND_PACKAGE(OpenMP)
ELSE(OPENMP_VERSION_CMAKE)
    MESSAGE(STATUS "CMake version is too old: cannot detected OpenMP. Some plugins may have a weaker performance without OpenMP.")
ENDIF(OPENMP_VERSION_CMAKE)

INCLUDE(MacroOptionalDependPackage)

# ==================================================================================================
# Log messages

MESSAGE(STATUS "")
MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS " kipi-plugins ${KIPIPLUGINS_VERSION_STRING} dependencies results   <http://www.kipi-plugins.org>")
MESSAGE(STATUS "")

# Require shared libraries results.

PRINT_LIBRARY_STATUS("libjpeg" "http://www.ijg.org" "" JPEG_FOUND)
PRINT_LIBRARY_STATUS("libtiff" "http://www.remotesensing.org/libtiff" "" TIFF_FOUND)
PRINT_LIBRARY_STATUS("libpng" "http://www.libpng.org/pub/png/libpng.html" " (version >= 1.2.7)" PNG_FOUND)
PRINT_LIBRARY_STATUS("libkipi" "http://www.digikam.org/sharedlibs" " (version >= 0.2.0)" KIPI_FOUND)
PRINT_LIBRARY_STATUS("libkexiv2" "http://www.digikam.org/sharedlibs" " (version >= 0.2.0)" KEXIV2_FOUND)
PRINT_LIBRARY_STATUS("libkdcraw" "http://www.digikam.org/sharedlibs" " (version >= 1.2.0)" KDCRAW_FOUND)

# Optional shared libraries results.

PRINT_OPTIONAL_LIBRARY_STATUS("libxml2" LIBXML2_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libxslt" LIBXSLT_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libexpat" EXPAT_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("native threads support" CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
PRINT_OPTIONAL_LIBRARY_STATUS("libopengl" OPENGL_FOUND AND OPENGL_GLU_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libopencv" OPENCV_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libgpod" GPOD_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("Gdk" GDK_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkdepim" KDEPIMLIBS_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("qca2" QCA2_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkdcraw >= 1.0.0" KDCRAW_FOR_EXPOBLENDING)

IF(OPENMP_VERSION_CMAKE)
    PRINT_OPTIONAL_LIBRARY_STATUS("OpenMP" OPENMP_FOUND)
ENDIF(OPENMP_VERSION_CMAKE)

# LINUX Only.
IF(NOT WIN32 AND NOT APPLE)
    PRINT_OPTIONAL_LIBRARY_STATUS("libX11" X11_FOUND AND X11_Xrandr_FOUND)
ENDIF(NOT WIN32 AND NOT APPLE)

PRINT_OPTIONAL_LIBRARY_STATUS("libksane" KSANE_FOUND)

# Optional plugins results ----------------------------------------------------------------------------

MESSAGE(STATUS "")

IF(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOR_KIPIPLUGINS AND KEXIV2_FOUND AND KIPI_FOUND)
    SET(KIPI_COMPILE_MESSAGE "kipi-plugins will be compiled")
    FILL_WITH_DOTS(KIPI_COMPILE_MESSAGE)
    MESSAGE(STATUS " ${KIPI_COMPILE_MESSAGE} YES")
    SET(KIPIPLUGINS_CAN_BE_COMPILED true)
ELSE(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOR_KIPIPLUGINS AND KEXIV2_FOUND AND KIPI_FOUND)
    MESSAGE(FATAL_ERROR " kipi-plugins will not be compiled! Look README file for more details about dependencies...")
    SET(KIPIPLUGINS_CAN_BE_COMPILED false)
ENDIF(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOR_KIPIPLUGINS AND KEXIV2_FOUND AND KIPI_FOUND)

PRINT_PLUGIN_COMPILE_STATUS("Shwup" QCA2_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("HtmlExport" LIBXML2_FOUND AND LIBXSLT_FOUND)

IF(NOT WIN32 AND NOT APPLE)
    PRINT_PLUGIN_COMPILE_STATUS("AdvancedSlideshow" X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND)
    PRINT_PLUGIN_COMPILE_STATUS("ImageViewer" X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND)
ELSE(NOT WIN32 AND NOT APPLE)
    PRINT_PLUGIN_COMPILE_STATUS("AdvancedSlideshow" OPENGL_FOUND AND OPENGL_GLU_FOUND)
    PRINT_PLUGIN_COMPILE_STATUS("ImageViewer" OPENGL_FOUND AND OPENGL_GLU_FOUND)
ENDIF(NOT WIN32 AND NOT APPLE)

PRINT_PLUGIN_COMPILE_STATUS("AcquireImages" KSANE_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("DNGConverter" EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)
PRINT_PLUGIN_COMPILE_STATUS("RemoveRedEyes" OPENCV_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("IpodExport" GPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("Calendar" KDEPIMLIBS_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("ExposureBlending" KDCRAW_FOR_EXPOBLENDING)

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
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/dialogs
                        ${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins/dialogs
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/widgets
                        ${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins/widgets
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/tools
                        ${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins/tools
                        ${KEXIV2_INCLUDE_DIR}
                        ${KDCRAW_INCLUDE_DIR}
                        ${KIPI_INCLUDE_DIR}
                        ${KDE4_INCLUDES}
                        ${QT4_INCLUDES}
                       )

    # debug settings
    SET(AREA_CODE_GENERAL "51000")
    SET(AREA_CODE_LOADING "51001")
    ADD_DEFINITIONS(-DKDE_DEFAULT_DEBUG_AREA=${AREA_CODE_GENERAL})
    ADD_DEFINITIONS(-DAREA_CODE_GENERAL=${AREA_CODE_GENERAL})
    ADD_DEFINITIONS(-DAREA_CODE_LOADING=${AREA_CODE_LOADING})

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
    ADD_SUBDIRECTORY(piwigoexport)
    ADD_SUBDIRECTORY(kioexportimport)
    ADD_SUBDIRECTORY(picasawebexport)
    ADD_SUBDIRECTORY(smug)
    ADD_SUBDIRECTORY(facebook)
    ADD_SUBDIRECTORY(printimages)
    ADD_SUBDIRECTORY(expoblending)

    # Plugins which are optional depencies.

    IF(KDEPIMLIBS_FOUND)
       ADD_SUBDIRECTORY(calendar)
    ENDIF(KDEPIMLIBS_FOUND)

    IF(QCA2_FOUND)
       ADD_SUBDIRECTORY(shwup)
    ENDIF(QCA2_FOUND)

    IF(GPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)
       ADD_SUBDIRECTORY(ipodexport)
    ENDIF(GPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)

    IF(OPENCV_FOUND)
        ADD_SUBDIRECTORY(removeredeyes)
    ENDIF(OPENCV_FOUND)

    IF(LIBXML2_FOUND AND LIBXSLT_FOUND)
        ADD_SUBDIRECTORY(htmlexport)
    ENDIF(LIBXML2_FOUND AND LIBXSLT_FOUND)

    IF(KSANE_FOUND)
        ADD_SUBDIRECTORY(acquireimages)
    ENDIF(KSANE_FOUND)

    IF(NOT WIN32 AND NOT APPLE)

        # Plugins which depend on OpenGL
        IF(X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND)
            # Under LINUX we need X11 library with OpenGL.
            ADD_SUBDIRECTORY(advancedslideshow)
            ADD_SUBDIRECTORY(imageviewer)
        ENDIF(X11_FOUND AND X11_Xrandr_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND)

    ELSE(NOT WIN32 AND NOT APPLE)

        # Plugins which depend on OpenGL
        IF(OPENGL_FOUND AND OPENGL_GLU_FOUND)
            ADD_SUBDIRECTORY(advancedslideshow)
            ADD_SUBDIRECTORY(imageviewer)
        ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND)

    ENDIF(NOT WIN32 AND NOT APPLE)

    IF(EXPAT_FOUND)
        IF(CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
            ADD_SUBDIRECTORY(dngconverter)
        ENDIF(CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
    ENDIF(EXPAT_FOUND)

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
