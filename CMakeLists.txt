PROJECT(kipi-plugins)

MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS "Starting CMake configuration for: kipi-plugins")

# =======================================================
# Information to update before to release this package.

# kipi-plugins version
SET(KIPIPLUGINS_MAJOR_VERSION "2")
SET(KIPIPLUGINS_MINOR_VERSION "6")
SET(KIPIPLUGINS_PATCH_VERSION "0")

# kipi-plugins release date
SET (KIPIPLUGINS_RELEASE_DATE "2012-06-03")

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

MACRO(PRINT_OPTIONAL_QTMODULE_STATUS NAME)

    SET(LIB_MESSAGE "${NAME} module found")
    FILL_WITH_DOTS(LIB_MESSAGE)

    IF(${ARGN})
        MESSAGE(STATUS " ${LIB_MESSAGE} YES")
    ELSE(${ARGN})
        MESSAGE(STATUS " ${LIB_MESSAGE} NO")
    ENDIF(${ARGN})

ENDMACRO(PRINT_OPTIONAL_QTMODULE_STATUS)

# =======================================================

CMAKE_POLICY(SET CMP0017 NEW)

FIND_PACKAGE(KDE4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

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

# check the libkdcraw version
# note that old versions don't specify version information, so for now
# just inform the user if version information was found
SET(KDCRAW_MIN_VERSION "1.1.0")
IF (KDCRAW_VERSION)
    MESSAGE(STATUS "libkdcraw: Found version ${KDCRAW_VERSION} (required: ${KDCRAW_MIN_VERSION})")
    IF (${KDCRAW_VERSION} VERSION_LESS ${KDCRAW_MIN_VERSION})
        SET(KDCRAW_FOUND FALSE)
    ENDIF (${KDCRAW_VERSION} VERSION_LESS ${KDCRAW_MIN_VERSION})
ELSE (KDCRAW_VERSION)
    MESSAGE(WARNING "libkdcraw: Version information not found, your version is probably too old.")
ENDIF (KDCRAW_VERSION)

# Optional depencies detection required by some plugins

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

MACRO_OPTIONAL_FIND_PACKAGE(EXPAT)         # For DNGConverter: XMP SDK need Expat library to compile.
MACRO_OPTIONAL_FIND_PACKAGE(Threads)       # For DNGConverter: DNG SDK need native threads support.
MACRO_OPTIONAL_FIND_PACKAGE(LibXml2)       # For Htmlexport.
MACRO_OPTIONAL_FIND_PACKAGE(LibXslt)       # For Htmlexport.
MACRO_OPTIONAL_FIND_PACKAGE(OpenGL)        # For AdvancedSlideshow and ImageViewer.
MACRO_OPTIONAL_FIND_PACKAGE(Ipod)          # For ipodexport.
MACRO_OPTIONAL_FIND_PACKAGE(Gdk)           # For ipodexport.
MACRO_OPTIONAL_FIND_PACKAGE(GLIB2)         # For ipodexport.
MACRO_OPTIONAL_FIND_PACKAGE(GObject)       # For ipodexport.
MACRO_OPTIONAL_FIND_PACKAGE(KdepimLibs)    # For Calendar (libkcal).
MACRO_OPTIONAL_FIND_PACKAGE(QCA2)          # For Shwup and YandexFotki.
MACRO_OPTIONAL_FIND_PACKAGE(KSane)         # For AcquireImages.
MACRO_OPTIONAL_FIND_PACKAGE(OpenCV)        # For RemoveRedEyes.
MACRO_OPTIONAL_FIND_PACKAGE(QJSON)         # For Debian Screenshots and Facebook, and Imgur.
MACRO_OPTIONAL_FIND_PACKAGE(KGeoMap)       # For GPSSync.
MACRO_OPTIONAL_FIND_PACKAGE(Mediawiki)     # For Mediawiki.
MACRO_OPTIONAL_FIND_PACKAGE(LibKVkontakte) # For VKontakte.
MACRO_OPTIONAL_FIND_PACKAGE(Boost)         # For Panorama

# check the libkgeomap version
# note that old versions don't specify version information, so for now
# just inform the user if version information was found
IF (KGEOMAP_VERSION)
    MESSAGE(STATUS "libkgeomap: Found version ${KGEOMAP_VERSION}")
    # TODO: fail if the version is too low
ENDIF (KGEOMAP_VERSION)

# X11 check, LINUX Only.
IF(NOT WIN32 AND NOT APPLE)
    MACRO_OPTIONAL_FIND_PACKAGE(X11)   # For AdvancedSlideshow and ImageViewer.
ENDIF(NOT WIN32 AND NOT APPLE)

# SPECIAL OPENCV HANDLING

# there are two versions of FindOpenCV.cmake in the wild, one defining
# OpenCV_LIBRARIES, the other defining OpenCV_LIBS. Make sure we handle
# both cases.
IF (NOT OpenCV_LIBRARIES)
    SET(OpenCV_LIBRARIES ${OpenCV_LIBS})
ENDIF (NOT OpenCV_LIBRARIES)

# same story with OpenCV_INCLUDE_DIRS and OpenCV_INCLUDE_DIR:
IF (NOT OpenCV_INCLUDE_DIRS)
    SET(OpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIR})
ENDIF (NOT OpenCV_INCLUDE_DIRS)

IF (OpenCV_LIBRARIES AND OpenCV_INCLUDE_DIRS)
    SET (OpenCV_FOUND TRUE)
ENDIF (OpenCV_LIBRARIES AND OpenCV_INCLUDE_DIRS)

# OpenMP detection requires CMake >= 2.6.3

EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} --version OUTPUT_VARIABLE VERSION_CMAKE_INFO)
MESSAGE(STATUS "CMake version: ${VERSION_CMAKE_INFO}")
STRING(REPLACE "-patch " "." VERSION_CMAKE_INFO "${VERSION_CMAKE_INFO}")
MESSAGE(STATUS "CMake version (cleaned): ${VERSION_CMAKE_INFO}")
MACRO_ENSURE_VERSION("2.6.3" "${VERSION_CMAKE_INFO}" OPENMP_VERSION_CMAKE)

IF(OPENMP_VERSION_CMAKE AND NOT APPLE)
    MACRO_OPTIONAL_FIND_PACKAGE(OpenMP)
ELSE(OPENMP_VERSION_CMAKE AND NOT APPLE)
    MESSAGE(STATUS "CMake version is too old: cannot detected OpenMP. Some plugins may have a weaker performance without OpenMP.")
ENDIF(OPENMP_VERSION_CMAKE AND NOT APPLE)

INCLUDE(MacroOptionalDependPackage)

# ==================================================================================================
# Log messages

MESSAGE(STATUS "")
MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS " kipi-plugins ${KIPIPLUGINS_VERSION_STRING} dependencies results   <http://www.digikam.org>")
MESSAGE(STATUS "")

# Require shared libraries results.

PRINT_LIBRARY_STATUS("libjpeg" "http://www.ijg.org" "" JPEG_FOUND)
PRINT_LIBRARY_STATUS("libtiff" "http://www.remotesensing.org/libtiff" "" TIFF_FOUND)
PRINT_LIBRARY_STATUS("libpng" "http://www.libpng.org/pub/png/libpng.html" " (version >= 1.2.7)" PNG_FOUND)
PRINT_LIBRARY_STATUS("libkipi" "http://www.digikam.org/sharedlibs" " (version >= 1.2.0)" KIPI_FOUND)
PRINT_LIBRARY_STATUS("libkexiv2" "http://www.digikam.org/sharedlibs" " (version >= 1.2.0)" KEXIV2_FOUND)
PRINT_LIBRARY_STATUS("libkdcraw" "http://www.digikam.org/sharedlibs" " (version >= ${KDCRAW_MIN_VERSION})" KDCRAW_FOUND)

# Optional shared libraries results.

PRINT_OPTIONAL_LIBRARY_STATUS("libxml2" LIBXML2_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libxslt" LIBXSLT_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libexpat" EXPAT_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("native threads support" CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT)
PRINT_OPTIONAL_LIBRARY_STATUS("libopengl" OPENGL_FOUND AND OPENGL_GLU_FOUND)
PRINT_OPTIONAL_QTMODULE_STATUS("Qt4 OpenGL" QT_QTOPENGL_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libopencv" OpenCV_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("QJson" QJSON_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libgpod" IPOD_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("Gdk" GDK_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkdepim" KDEPIMLIBS_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("qca2" QCA2_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkgeomap" KGEOMAP_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libmediawiki" MEDIAWIKI_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("libkvkontakte" LIBKVKONTAKTE_FOUND)
PRINT_OPTIONAL_LIBRARY_STATUS("boost" Boost_FOUND)

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

IF(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)
    SET(KIPI_COMPILE_MESSAGE "kipi-plugins will be compiled")
    FILL_WITH_DOTS(KIPI_COMPILE_MESSAGE)
    MESSAGE(STATUS " ${KIPI_COMPILE_MESSAGE} YES")
    SET(KIPIPLUGINS_CAN_BE_COMPILED true)
ELSE(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)
    MESSAGE(FATAL_ERROR " kipi-plugins will not be compiled! Look README file for more details about dependencies...")
    SET(KIPIPLUGINS_CAN_BE_COMPILED false)
ENDIF(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)

PRINT_PLUGIN_COMPILE_STATUS("Shwup" QCA2_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("YandexFotki" QCA2_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("HtmlExport" LIBXML2_FOUND AND LIBXSLT_FOUND)

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

PRINT_PLUGIN_COMPILE_STATUS("AdvancedSlideshow" BUILD_VIEWERS)
PRINT_PLUGIN_COMPILE_STATUS("ImageViewer" BUILD_VIEWERS)
PRINT_PLUGIN_COMPILE_STATUS("AcquireImages" KSANE_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("DNGConverter" EXPAT_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("RemoveRedEyes" OpenCV_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("Debian Screenshots" QJSON_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("Facebook" QJSON_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("Imgur" QJSON_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("VKontakte" LIBKVKONTAKTE_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("IpodExport" IPOD_FOUND AND GLIB2_FOUND AND GOBJECT_FOUND AND GDK_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("Calendar" KDEPIMLIBS_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("GPSSync" KGEOMAP_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("Mediawiki" MEDIAWIKI_FOUND)
PRINT_PLUGIN_COMPILE_STATUS("Panorama" Boost_FOUND)

MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS "")

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
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/gitscript.cmake.in_cmake"
                   "${CMAKE_CURRENT_BINARY_DIR}/gitscript.cmake"
                   @ONLY)

    # Add a custom target to drive the custom command.
    ADD_CUSTOM_TARGET(kipiplugins-gitversion
                      ALL COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/gitscript.cmake")

    # ==================================================================================================

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins
                        ${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/dialogs
                        ${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins/dialogs
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/widgets
                        ${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins/widgets
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/tools
                        ${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins/tools
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/tools/threads
                        ${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins/tools/threads
                        ${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins/tools/imageio
                        ${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins/tools/imageio
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

    # Plugins which are optional depencies.

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
    ENDIF(QJSON_FOUND)

    IF(LIBKVKONTAKTE_FOUND)
        ADD_SUBDIRECTORY(vkontakte)
    ENDIF(LIBKVKONTAKTE_FOUND)

    IF(LIBXML2_FOUND AND LIBXSLT_FOUND)
        ADD_SUBDIRECTORY(htmlexport)
    ENDIF(LIBXML2_FOUND AND LIBXSLT_FOUND)

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

    IF(Boost_FOUND)
        ADD_SUBDIRECTORY(panorama)
    ENDIF(Boost_FOUND)

#   Laurent Montel : wallpaper is currently disabled. Waiting new api with KDE 4.2
#
#   FIND_FILE(KDESKTOP_INTERFACE_FILE_FOUND org.kde.kdesktop.Background.xml PATHS ${DBUS_INTERFACES_INSTALL_DIR} )
#
#   IF(KDESKTOP_INTERFACE_FILE_FOUND)
#       ADD_SUBDIRECTORY( wallpaper )
#   ELSE(KDESKTOP_INTERFACE_FILE_FOUND)
#       MESSAGE(STATUS "Wallpaper plugins will not compile. Kdebase (kdesktop) must be installed before compiling it")
#   ENDIF(KDESKTOP_INTERFACE_FILE_FOUND)

    IF(NOT WIN32)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/kipi-plugins.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipi-plugins.lsm)
    ENDIF(NOT WIN32)

ENDIF(KIPIPLUGINS_CAN_BE_COMPILED)
