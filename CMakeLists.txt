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

# Depencies detection require by all plugins
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
FIND_PACKAGE(Kexiv2 REQUIRED)
FIND_PACKAGE(Kdcraw REQUIRED)
FIND_PACKAGE(Ksane REQUIRED)
FIND_PACKAGE(Kipi REQUIRED)
FIND_PACKAGE(JPEG REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(TIFF REQUIRED)
macro_optional_find_package(Sane)

# Optional depencies detection require by any plugins
#MACRO_OPTIONAL_FIND_PACKAGE(Gphoto2)
#MACRO_OPTIONAL_FIND_PACKAGE(OpenGL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/libkipiplugins 
                    ${CMAKE_CURRENT_BINARY_DIR}/common/libkipiplugins
                    ${KEXIV2_INCLUDE_DIR} 
                    ${KDCRAW_INCLUDE_DIR} 
                    ${KSANE_INCLUDE_DIR} 
                    ${KIPI_INCLUDE_DIR} 
                    ${KDE4_INCLUDES} 
                    ${QT4_INCLUDES}
	           )

# TODO: others optional depencies to check when others plugins will be ported to KDE4
#
# Plugins witch depand of GPhoto2
#IF(KDE4_GPHOTO2 AND GPHOTO2_FOUND)
#	macro_optional_add_subdirectory(kameraklient)
#ENDIF(KDE4_GPHOTO2 AND GPHOTO2_FOUND)
#
# Plugins witch depand of OpenGL
#IF(OPENGL_FOUND AND OPENGL_GLU_FOUND)
#   MACRO_OPTIONAL_ADD_SUBDIRECTORY(slideshow)
#   MACRO_OPTIONAL_ADD_SUBDIRECTORY(imageviewer)
#ENDIF(OPENGL_FOUND AND OPENGL_GLU_FOUND)
#
# htmlexport need Xslt library
# slideshow need Imlib2 library
# ipodexport need Gpod library
# calendar need Kcal library (KDEPIM)
      

#find_file(KDESKTOP_INTERFACE_FILE_FOUND org.kde.kdesktop.Background.xml PATHS ${DBUS_INTERFACES_INSTALL_DIR} )

#IF(KDESKTOP_INTERFACE_FILE_FOUND)
#        add_subdirectory( wallpaper )
#ELSE(KDESKTOP_INTERFACE_FILE_FOUND)
#        MESSAGE(STATUS "Wallpaper plugins will not compile. Kdebase (kdesktop) must be installed before compiling it")
#ENDIF(KDESKTOP_INTERFACE_FILE_FOUND)

# Plugins witch will be always compiled
ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(timeadjust)
ADD_SUBDIRECTORY(jpeglossless)
ADD_SUBDIRECTORY(rawconverter)
ADD_SUBDIRECTORY(metadataedit)
ADD_SUBDIRECTORY(sendimages)
if(SANE_FOUND)
   ADD_SUBDIRECTORY(acquireimages)
endif(SANE_FOUND)
IF(NOT WIN32)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/kipi-plugins.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipi-plugins.lsm)
ENDIF(NOT WIN32)
