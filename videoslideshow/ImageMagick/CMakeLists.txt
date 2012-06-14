
find_package(ImageMagick COMPONENTS MagickCore)

if(ImageMagick_FOUND )
    include(${QT_USE_FILE})
    include_directories(${ImageMagick_INCLUDE_DIRS} ${QT_INCLUDES} 
                        ${KDE4_INCLUDES}
                        ${CMAKE_BINARY_DIR})

    add_subdirectory( TestMagick )

    set(imagemagick_SRCS  magick_api.cpp
        main.cpp 
        processimage.cpp
    )

    kde4_add_executable(imagemagick ${imagemagick_SRCS})

    target_link_libraries(imagemagick ${ImageMagick_LIBRARIES} ${QT_LIBRARIES})

    install(TARGETS imagemagick ${INSTALL_TARGETS_DEFAULT_ARGS} )
endif(ImageMagick_FOUND )

