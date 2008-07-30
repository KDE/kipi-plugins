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
SET(KIPIPLUGINS_SUFFIX_VERSION "-svn")

# =======================================================
# Set env. variables accordinly.

SET(KIPIPLUGINS_VERSION_STRING "${KIPIPLUGINS_MAJOR_VERSION}.${KIPIPLUGINS_MINOR_VERSION}.${KIPIPLUGINS_PATCH_VERSION}${KIPIPLUGINS_SUFFIX_VERSION}")

# =======================================================

INCLUDE(FindPackageHandleStandardArgs)

# Depencies detection required by all plugins
FIND_PACKAGE(Kexiv2 REQUIRED)
FIND_PACKAGE(Kdcraw REQUIRED)
FIND_PACKAGE(Kipi REQUIRED)
FIND_PACKAGE(JPEG REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(TIFF REQUIRED)

# Optional depencies detection required by some plugins

MACRO_OPTIONAL_FIND_PACKAGE(Sane)    # For AcquireImages
MACRO_OPTIONAL_FIND_PACKAGE(X11)     # For Slideshow
MACRO_OPTIONAL_FIND_PACKAGE(OpenGL)  # For Slideshow and ImageViewer
MACRO_OPTIONAL_FIND_PACKAGE(LibXml2) # For Htmlexport
MACRO_OPTIONAL_FIND_PACKAGE(LibXslt) # For Htmlexport

#SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
INCLUDE(MacroOptionalDependPackage)
MACRO_OPTIONAL_DEPEND_PACKAGE(KSane "ksane")
IF(DEPEND_PACKAGE_KSane)
    MACRO_OPTIONAL_FIND_PACKAGE(KSane)   # For AcquireImages
ELSE(DEPEND_PACKAGE_KSane)
    set (KSANE_FOUND FALSE)
ENDIF(DEPEND_PACKAGE_KSane)


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
#ADD_SUBDIRECTORY(galleryexport)

if(SANE_FOUND AND KSANE_FOUND)
   ADD_SUBDIRECTORY(acquireimages)
endif(SANE_FOUND AND KSANE_FOUND)

if (LIBXML2_FOUND AND LIBXSLT_FOUND)
   ADD_SUBDIRECTORY(htmlexport)
endif(LIBXML2_FOUND AND LIBXSLT_FOUND)

# Plugins which depend on OpenGL

IF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND AND X11_Xrandr_FOUND)
   MACRO_OPTIONAL_ADD_SUBDIRECTORY(slideshow)
ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND AND X11_Xrandr_FOUND)

IF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND)
   MACRO_OPTIONAL_ADD_SUBDIRECTORY(imageviewer)
ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND AND X11_FOUND)

# TODO: add others optional depencies to check when others plugins will be ported to KDE4
#
# ipodexport need Gpod library
# calendar need Kcal library (KDEPIM)
#      
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
