ADD_SUBDIRECTORY(test)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(${ImageMagick_INCLUDE_DIRS}
                    ${QT_INCLUDES}
                    ${KDE4_INCLUDES}
                    ${CMAKE_BINARY_DIR}
                   )

SET(imagemagick_SRCS
    magick_api.cpp
    main.cpp
    processimage.cpp
   )

KDE4_ADD_EXECUTABLE(imagemagick ${imagemagick_SRCS})

TARGET_LINK_LIBRARIES(imagemagick ${ImageMagick_LIBRARIES} ${QT_LIBRARIES})

INSTALL(TARGETS imagemagick ${INSTALL_TARGETS_DEFAULT_ARGS})
