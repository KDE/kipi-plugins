ADD_SUBDIRECTORY(test)

INCLUDE_DIRECTORIES(${ImageMagick_INCLUDE_DIRS}
                    ${QT_INCLUDES}
                    ${KDE4_INCLUDES}
                    ${CMAKE_BINARY_DIR}
                   )

SET(magickiface_SRCS
    magickiface.cpp
    processimage.cpp
   )

KDE4_ADD_LIBRARY(magickiface STATIC ${magickiface_SRCS})

TARGET_LINK_LIBRARIES(magickiface
                      ${QT_QTGUI_LIBRARY}
                      ${KDE4_KDECORE_LIBS}
                      ${ImageMagick_LIBRARIES}
                     )
