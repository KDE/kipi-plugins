project(kipi-plugins)

# Depencies detection require by all plugins
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
find_package(Kexiv2 REQUIRED)
find_package(Kdcraw REQUIRED)
find_package(Kipi REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(TIFF REQUIRED)

# Optional depencies detection require by any plugins
macro_optional_find_package(Gphoto2)
macro_optional_find_package(OpenGL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/include ${KEXIV2_INCLUDE_DIR} ${KDCRAW_INCLUDE_DIR} ${KIPI_INCLUDE_DIR} ${KDE4_INCLUDES} ${QT4_INCLUDES})

# Plugins witch depand of OpenGL
if(OPENGL_FOUND AND OPENGL_GLU_FOUND)
   macro_optional_add_subdirectory(slideshow)
   macro_optional_add_subdirectory(imageviewer)
endif(OPENGL_FOUND AND OPENGL_GLU_FOUND)

# Plugins witch depand of GPhoto2
if(KDE4_GPHOTO2 AND GPHOTO2_FOUND)
   macro_optional_add_subdirectory(kameraklient)
endif(KDE4_GPHOTO2 AND GPHOTO2_FOUND)

# TODO: others optional depencies to check
# htmlexport need Xslt library
# slideshow need Imlib2 library
# ipodexport need Gpod library
# calendar need Kcal library (KDEPIM)
      
# Plugins witch will be always compiled
add_subdirectory(timeadjust)

find_file(KDESKTOP_INTERFACE_FILE_FOUND org.kde.kdesktop.Background.xml PATHS ${DBUS_INTERFACES_DIR} )

if(KDESKTOP_INTERFACE_FILE_FOUND)
        add_subdirectory( wallpaper )
else(KDESKTOP_INTERFACE_FILE_FOUND)
        MESSAGE(STATUS "Wallpaper plugins will not compile. Kdebase (kdesktop) must be installed before compiling it")
endif(KDESKTOP_INTERFACE_FILE_FOUND)

add_subdirectory(jpeglossless)

