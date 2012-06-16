ADD_SUBDIRECTORY(test)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(${ImageMagick_INCLUDE_DIRS}
                    ${QT_INCLUDES}
                    ${KDE4_INCLUDES}
                    ${CMAKE_BINARY_DIR}
                   )

SET(magickiface_SRCS
    magick_api.cpp
    processimage.cpp
   )

ADD_LIBRARY(magickiface ${magickiface_SRCS})

TARGET_LINK_LIBRARIES(magickiface ${ImageMagick_LIBRARIES} ${QT_LIBRARIES})
