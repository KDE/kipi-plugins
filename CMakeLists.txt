PROJECT(kipi-plugins)

# =======================================================
# Informations to update before to release this package.

# kipi-plugins version
SET(KIPIPLUGINS_MAJOR_VERSION "0")
SET(KIPIPLUGINS_MINOR_VERSION "2")
SET(KIPIPLUGINS_PATCH_VERSION "0")

# Suffix to add at end of version string. Usual values are:
# "-svn"   : alpha code unstable from svn. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
SET(KIPIPLUGINS_SUFFIX_VERSION "-beta2")

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

MACRO_OPTIONAL_FIND_PACKAGE(Sane)    # For AcquireImages
MACRO_OPTIONAL_FIND_PACKAGE(EXPAT)   # For DNGConverter: XMP SDK need Expat library to compile.
MACRO_OPTIONAL_FIND_PACKAGE(LibXml2) # For Htmlexport
MACRO_OPTIONAL_FIND_PACKAGE(LibXslt) # For Htmlexport
MACRO_OPTIONAL_FIND_PACKAGE(OpenGL)  # For Slideshow and ImageViewer
MACRO_OPTIONAL_FIND_PACKAGE(X11)     # For Slideshow

# Check KDcraw version installed to compile fine DngConverter plugin.
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(Kdcraw libkdcraw>=0.4.0)
IF(Kdcraw_FOUND)
    SET(KDCRAW_FOR_DNGCONVERTER TRUE)
ELSE(Kdcraw_FOUND)
    SET(KDCRAW_FOR_DNGCONVERTER FALSE)
ENDIF(Kdcraw_FOUND)

INCLUDE(MacroOptionalDependPackage)
MACRO_OPTIONAL_DEPEND_PACKAGE(KSane "ksane")
IF(DEPEND_PACKAGE_KSane)
    MACRO_OPTIONAL_FIND_PACKAGE(KSane)   # For AcquireImages
ELSE(DEPEND_PACKAGE_KSane)
    SET(KSANE_FOUND FALSE)
ENDIF(DEPEND_PACKAGE_KSane)

# TODO: add others optional depencies to check when others plugins will be ported to KDE4
#
# ipodexport need Gpod library
# calendar need Kcal library (KDEPIM)

# ==================================================================================================
# Log messages

MESSAGE(STATUS "")
MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS " kipi-plugins ${KIPIPLUGINS_VERSION_STRING} dependencies results")
MESSAGE(STATUS "")

IF(JPEG_FOUND)
    MESSAGE(STATUS " libjpeg library found.......... YES")
ELSE(JPEG_FOUND)
    MESSAGE(STATUS " libjpeg library found.......... NO")
    MESSAGE(STATUS "")
    MESSAGE(SEND_ERROR " digiKam needs libjpeg. You need to install the libjpeg development package.")
    MESSAGE(STATUS " libjpeg website is at http://www.ijg.org")
    MESSAGE(STATUS "")
ENDIF(JPEG_FOUND)

IF(TIFF_FOUND)
    MESSAGE(STATUS " libtiff library found.......... YES")
ELSE(TIFF_FOUND)
    MESSAGE(STATUS " libtiff library found.......... NO")
    MESSAGE(STATUS "")
    MESSAGE(SEND_ERROR " kipi-plugins needs libtiff. You need to install the libtiff development package.")
    MESSAGE(STATUS " libtiff website is at http://www.remotesensing.org/libtiff")
    MESSAGE(STATUS "")
ENDIF(TIFF_FOUND)

IF(PNG_FOUND)
    MESSAGE(STATUS " libpng library found........... YES")
ELSE(PNG_FOUND)
    MESSAGE(STATUS " libpng library found........... NO")
    MESSAGE(STATUS "")
    MESSAGE(SEND_ERROR " kipi-plugins needs libpng. You need to install the correct version (>= 1.2.7).")
    MESSAGE(STATUS " Libpng website is at http://www.libpng.org/pub/png/libpng.html")
    MESSAGE(STATUS "")
ENDIF(PNG_FOUND)

IF(KIPI_FOUND)
    MESSAGE(STATUS " libkipi library found.......... YES")
ELSE(KIPI_FOUND)
    MESSAGE(STATUS " libkipi library found.......... NO")
    MESSAGE(STATUS "")
    MESSAGE(SEND_ERROR " kipi-plugins needs libkipi library >= 0.2.0. You need to install libkipi first")
    MESSAGE(STATUS " libkipi website is at http://www.kipi-plugins.org")
    MESSAGE(STATUS "")
ENDIF(KIPI_FOUND)

IF(KEXIV2_FOUND)
    MESSAGE(STATUS " libkexiv2 library found........ YES")
ELSE(KEXIV2_FOUND)
    MESSAGE(STATUS " libkexiv2 library found........ NO")
    MESSAGE(STATUS "")
    MESSAGE(SEND_ERROR " kipi-plugins needs libkexiv2 library >= 0.2.0. You need to install libkexiv2 first")
    MESSAGE(STATUS " libkexiv2 website is at http://www.kipi-plugins.org")
    MESSAGE(STATUS "")
ENDIF(KEXIV2_FOUND)

IF(KDCRAW_FOUND)
    MESSAGE(STATUS " libkdcraw library found........ YES")
ELSE(KDCRAW_FOUND)
    MESSAGE(STATUS " libkdcraw library found........ NO")
    MESSAGE(STATUS "")
    MESSAGE(SEND_ERROR " kipi-plugins needs libkdcraw library >= 0.2.0. You need to install libkdcraw first")
    MESSAGE(STATUS " libkdcraw website is at http://www.kipi-plugins.org")
    MESSAGE(STATUS "")
ENDIF(KDCRAW_FOUND)

IF(KSANE_FOUND)
    MESSAGE(STATUS " libksane library found......... YES (optionnal)")
ELSE(KSANE_FOUND)
    MESSAGE(STATUS " libksane library found......... NO  (optionnal)")
ENDIF(KSANE_FOUND)

IF(LIBXML2_FOUND)
    MESSAGE(STATUS " libxml2 library found.......... YES (optionnal)")
ELSE(LIBXML2_FOUND)
    MESSAGE(STATUS " libxml2 library found.......... NO  (optionnal)")
ENDIF(LIBXML2_FOUND)

IF(LIBXSLT_FOUND)
    MESSAGE(STATUS " libxslt library found.......... YES (optionnal)")
ELSE(LIBXSLT_FOUND)
    MESSAGE(STATUS " libxslt library found.......... NO  (optionnal)")
ENDIF(LIBXSLT_FOUND)

IF(EXPAT_FOUND)
    MESSAGE(STATUS " libexpat library found......... YES (optionnal)")
ELSE(EXPAT_FOUND)
    MESSAGE(STATUS " libexpat library found......... NO  (optionnal)")
ENDIF(EXPAT_FOUND)

IF(OPENGL_FOUND AND OPENGL_GLU_FOUND)
    MESSAGE(STATUS " libopengl library found........ YES (optionnal)")
ELSE(OPENGL_FOUND AND OPENGL_GLU_FOUND)
    MESSAGE(STATUS " libopengl library found........ NO  (optionnal)")
ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND)

IF(X11_FOUND AND X11_Xrandr_FOUND)
    MESSAGE(STATUS " libX11 library found........... YES (optionnal)")
ELSE(X11_FOUND AND X11_Xrandr_FOUND)
    MESSAGE(STATUS " libX11 library found........... NO  (optionnal)")
ENDIF(X11_FOUND AND X11_Xrandr_FOUND)

IF(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)
    MESSAGE(STATUS " kipi-plugins will be compiled...")
    SET(KIPIPLUGINS_CAN_BE_COMPILED true)
ELSE(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)
    MESSAGE(FATAL_ERROR " kipi-plugins will not be compiled! Look README file for more details about dependencies...")
    SET(KIPIPLUGINS_CAN_BE_COMPILED false)
ENDIF(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)

IF(SANE_FOUND AND KSANE_FOUND)
    MESSAGE(STATUS " AcquireImages plugin will be compiled...")
ELSE(SANE_FOUND AND KSANE_FOUND)
    MESSAGE(STATUS " AcquireImages plugin will not be compiled! Look README file for more details about dependencies... ")
ENDIF(SANE_FOUND AND KSANE_FOUND)

IF(LIBXML2_FOUND AND LIBXSLT_FOUND)
    MESSAGE(STATUS " HtmlExport plugin will be compiled...")
ELSE(LIBXML2_FOUND AND LIBXSLT_FOUND)
    MESSAGE(STATUS " HtmlExport plugin will not be compiled! Look README file for more details about dependencies... ")
ENDIF(LIBXML2_FOUND AND LIBXSLT_FOUND)

IF(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)
    MESSAGE(STATUS " DNGConverter plugin will be compiled...")
ELSE(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)
    MESSAGE(STATUS " DNGConverter plugin will not be compiled! Look README file for more details about dependencies... ")
ENDIF(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)

IF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND AND X11_Xrandr_FOUND)
    MESSAGE(STATUS " SlideShow plugin will be compiled...")
ELSE(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND AND X11_Xrandr_FOUND)
    MESSAGE(STATUS " SlideShow plugin will not be compiled! Look README file for more details about dependencies... ")
ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND AND X11_Xrandr_FOUND)

IF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND)
    MESSAGE(STATUS " ImageViewer plugin will be compiled...")
ELSE(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND)
    MESSAGE(STATUS " ImageViewer plugin will not be compiled! Look README file for more details about dependencies... ")
ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND)

MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS "")

# ==================================================================================================

IF(KIPIPLUGINS_CAN_BE_COMPILED)

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
    ADD_SUBDIRECTORY(simpleviewerexport)
    ADD_SUBDIRECTORY(flickrexport)
    ADD_SUBDIRECTORY(picasawebexport)
    ADD_SUBDIRECTORY(galleryexport)

    IF(SANE_FOUND AND KSANE_FOUND)
        ADD_SUBDIRECTORY(acquireimages)
    ENDIF(SANE_FOUND AND KSANE_FOUND)

    IF(LIBXML2_FOUND AND LIBXSLT_FOUND)
        ADD_SUBDIRECTORY(htmlexport)
    ENDIF(LIBXML2_FOUND AND LIBXSLT_FOUND)

    IF(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)
        ADD_SUBDIRECTORY(dngconverter)
    ENDIF(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)

    # Plugins which depend on OpenGL

    IF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND AND X11_Xrandr_FOUND)
        MACRO_OPTIONAL_ADD_SUBDIRECTORY(slideshow)
    ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND AND X11_Xrandr_FOUND)

    IF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND)
        MACRO_OPTIONAL_ADD_SUBDIRECTORY(imageviewer)
    ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND)

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
