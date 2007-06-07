project(kipi-plugins)

# Depencies detection require by all plugins
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(TIFF REQUIRED)

# Optional depencies detection require by any plugins
macro_optional_find_package(Gphoto2)
macro_optional_find_package(OpenGL)

subdirs(wallpaper)

