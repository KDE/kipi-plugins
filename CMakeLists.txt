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

SET(KIPIPLUGINS_VERSION_STRING "${KIPIPLUGINS_MAJOR_VERSION}.${KIPIPLUGINS_MINOR_VERSION}.${KIPIPLUGINS_PATCH_VERSION}${KIPIPLUGINS_SUFFIX_VERSION}")

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

message(STATUS "")
message(STATUS "----------------------------------------------------------------------------------")
message(STATUS " kipi-plugins ${KIPIPLUGINS_VERSION_STRING} dependencies results")
message(STATUS "")

if(JPEG_FOUND)
  message(STATUS " libjpeg library found.......... YES")
else(JPEG_FOUND)
  message(STATUS " libjpeg library found.......... NO")
  message(STATUS "")
  message(SEND_ERROR " digiKam needs libjpeg. You need to install the libjpeg development package.")
  message(STATUS " libjpeg website is at http://www.ijg.org")
  message(STATUS "")
endif(JPEG_FOUND)

if(TIFF_FOUND)
  message(STATUS " libtiff library found.......... YES")
else(TIFF_FOUND)
  message(STATUS " libtiff library found.......... NO")
  message(STATUS "")
  message(SEND_ERROR " digiKam needs libtiff. You need to install the libtiff development package.")
  message(STATUS " libtiff website is at http://www.remotesensing.org/libtiff")
  message(STATUS "")
endif(TIFF_FOUND)

if(PNG_FOUND)
  message(STATUS " libpng library found........... YES")
else(PNG_FOUND)
  message(STATUS " libpng library found........... NO")
  message(STATUS "")
  message(SEND_ERROR " digiKam needs libpng. You need to install the correct version (>= 1.2.7).")
  message(STATUS " Libpng website is at http://www.libpng.org/pub/png/libpng.html")
  message(STATUS "")
endif(PNG_FOUND)

if(KIPI_FOUND)
  message(STATUS " libkipi library found.......... YES")
else(KIPI_FOUND)
  message(STATUS " libkipi library found.......... NO")
  message(STATUS "")
  message(SEND_ERROR " digiKam needs libkipi library >= 0.2.0. You need to install libkipi first")
  message(STATUS " libkipi website is at http://www.kipi-plugins.org")
  message(STATUS "")
endif(KIPI_FOUND)

if(KEXIV2_FOUND)
  message(STATUS " libkexiv2 library found........ YES")
else(KEXIV2_FOUND)
  message(STATUS " libkexiv2 library found........ NO")
  message(STATUS "")
  message(SEND_ERROR " digiKam needs libkexiv2 library >= 0.2.0. You need to install libkexiv2 first")
  message(STATUS " libkexiv2 website is at http://www.kipi-plugins.org")
  message(STATUS "")
endif(KEXIV2_FOUND)

if(KDCRAW_FOUND)
  message(STATUS " libkdcraw library found........ YES")
else(KDCRAW_FOUND)
  message(STATUS " libkdcraw library found........ NO")
  message(STATUS "")
  message(SEND_ERROR " digiKam needs libkdcraw library >= 0.2.0. You need to install libkdcraw first")
  message(STATUS " libkdcraw website is at http://www.kipi-plugins.org")
  message(STATUS "")
endif(KDCRAW_FOUND)

if(KSANE_FOUND)
  message(STATUS " libksane library found......... YES (optionnal)")
else(KSANE_FOUND)
  message(STATUS " libksane library found......... NO  (optionnal)")
endif(KSANE_FOUND)

if(LIBXML2_FOUND)
  message(STATUS " libxml2 library found.......... YES (optionnal)")
else(LIBXML2_FOUND)
  message(STATUS " libxml2 library found.......... NO  (optionnal)")
endif(LIBXML2_FOUND)

if(LIBXSLT_FOUND)
  message(STATUS " libxslt library found.......... YES (optionnal)")
else(LIBXSLT_FOUND)
  message(STATUS " libxslt library found.......... NO  (optionnal)")
endif(LIBXSLT_FOUND)

if(EXPAT_FOUND)
  message(STATUS " libexpat library found......... YES (optionnal)")
else(EXPAT_FOUND)
  message(STATUS " libexpat library found......... NO  (optionnal)")
endif(EXPAT_FOUND)

if(OPENGL_FOUND AND OPENGL_GLU_FOUND)
  message(STATUS " libopengl library found........ YES (optionnal)")
else(OPENGL_FOUND AND OPENGL_GLU_FOUND)
  message(STATUS " libopengl library found........ NO  (optionnal)")
endif(OPENGL_FOUND AND OPENGL_GLU_FOUND)

if(X11_FOUND AND X11_Xrandr_FOUND)
  message(STATUS " libX11 library found........... YES (optionnal)")
else(X11_FOUND AND X11_Xrandr_FOUND)
  message(STATUS " libX11 library found........... NO  (optionnal)")
endif(X11_FOUND AND X11_Xrandr_FOUND)

if(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)
  message(STATUS " kipi-plugins will be compiled...")
  set(KIPIPLUGINS_CAN_BE_COMPILED true)
else(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)
  message(FATAL_ERROR " kipi-plugins will not be compiled! Look README file for more details about dependencies...")
  set(KIPIPLUGINS_CAN_BE_COMPILED false)
endif(JPEG_FOUND AND PNG_FOUND AND TIFF_FOUND AND KDCRAW_FOUND AND KEXIV2_FOUND AND KIPI_FOUND)

if(SANE_FOUND AND KSANE_FOUND)
   message(STATUS " AcquireImages plugin will be compiled...")
else(SANE_FOUND AND KSANE_FOUND)
   message(STATUS " AcquireImages plugin will not be compiled! Look README file for more details about dependencies... ")
endif(SANE_FOUND AND KSANE_FOUND)

if(LIBXML2_FOUND AND LIBXSLT_FOUND)
   message(STATUS " HtmlExport plugin will be compiled...")
else(LIBXML2_FOUND AND LIBXSLT_FOUND)
   message(STATUS " HtmlExport plugin will not be compiled! Look README file for more details about dependencies... ")
endif(LIBXML2_FOUND AND LIBXSLT_FOUND)

if(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)
   message(STATUS " DNGConverter plugin will be compiled...")
else(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)
   message(STATUS " DNGConverter plugin will not be compiled! Look README file for more details about dependencies... ")
endif(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)

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

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "")

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

if(SANE_FOUND AND KSANE_FOUND)
   ADD_SUBDIRECTORY(acquireimages)
endif(SANE_FOUND AND KSANE_FOUND)

if(LIBXML2_FOUND AND LIBXSLT_FOUND)
   ADD_SUBDIRECTORY(htmlexport)
endif(LIBXML2_FOUND AND LIBXSLT_FOUND)

if(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)
   ADD_SUBDIRECTORY(dngconverter)
endif(EXPAT_FOUND AND KDCRAW_FOR_DNGCONVERTER)

# Plugins which depend on OpenGL

IF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND AND X11_Xrandr_FOUND)
   MACRO_OPTIONAL_ADD_SUBDIRECTORY(slideshow)
ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND AND X11_Xrandr_FOUND)

IF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND)
   MACRO_OPTIONAL_ADD_SUBDIRECTORY(imageviewer)
ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND)

# Laurent Montel : wallpaper is currently disabled. Waiting new api with KDE 4.2 
#
#find_file(KDESKTOP_INTERFACE_FILE_FOUND org.kde.kdesktop.Background.xml PATHS ${DBUS_INTERFACES_INSTALL_DIR} )
#
#IF(KDESKTOP_INTERFACE_FILE_FOUND)
#        add_subdirectory( wallpaper )
#ELSE(KDESKTOP_INTERFACE_FILE_FOUND)
#        MESSAGE(STATUS "Wallpaper plugins will not compile. Kdebase (kdesktop) must be installed before compiling it")
#ENDIF(KDESKTOP_INTERFACE_FILE_FOUND)

IF(NOT WIN32)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/kipi-plugins.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipi-plugins.lsm)
ENDIF(NOT WIN32)

ENDIF(KIPIPLUGINS_CAN_BE_COMPILED)
