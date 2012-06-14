find_package(ImageMagick COMPONENTS MagickCore display)

if(ImageMagick_FOUND )
    include(${QT_USE_FILE})
    include_directories(${ImageMagick_INCLUDE_DIRS} ${QT_INCLUDES} 
                        ${CMAKE_CURRENT_BINARY_DIR}/../ 
                        ${CMAKE_CURRENT_SOURCE_DIR}/../ )

    set(TestMagick_SRCS TestMagick.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../magick_api.cpp
     )

    kde4_add_executable(TestMagick ${TestMagick_SRCS} )

    target_link_libraries(TestMagick ${QT_QTTEST_LIBRARY} ${QT_LIBRARIES} ${ImageMagick_LIBRARIES} )

    ADD_TEST( NAME TestMagick COMMAND TestMagick )
endif(ImageMagick_FOUND )
  