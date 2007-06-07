project(kipi-plugins)

# Depencies detection require by all plugins
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(TIFF REQUIRED)

# TODO: others main depencies to check
# libkipi.
# libkdcraw.
# libkexiv2.

# Optional depencies detection require by any plugins
macro_optional_find_package(Gphoto2)
macro_optional_find_package(OpenGL)
macro_optional_find_package(KdePimLibs)

# Plugins witch depand of OpenGL
if(OPENGL_FOUND AND OPENGL_GLU_FOUND)
   macro_optional_add_subdirectory(slideshow)
   macro_optional_add_subdirectory(imageviewer)
endif(OPENGL_FOUND AND OPENGL_GLU_FOUND)

# Plugins witch depand of GPhoto2
if(KDE4_GPHOTO2 AND GPHOTO2_FOUND)
   macro_optional_add_subdirectory(kameraklient)
endif(KDE4_GPHOTO2 AND GPHOTO2_FOUND)

# Plugins witch depand of KDEPim Library
if(KDEPIMLIBS_FOUND)
   macro_optional_add_subdirectory(calendar)
endif(KDEPIMLIBS_FOUND)

# TODO: others optional depencies to check
# htmlexport need XSLT library
# slideshow need Imlib2 library
# ipodexport need Gpod library
      
# Plugins witch will be always compiled
subdirs(wallpaper)

