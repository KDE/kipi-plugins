
INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(${ImageMagick_INCLUDE_DIRS}
                    ${QT_INCLUDES}
                    ${CMAKE_CURRENT_BINARY_DIR}/../
                    ${CMAKE_CURRENT_SOURCE_DIR}/../
                   )

SET(testmagickiface_SRCS testmagickiface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../magick_api.cpp)

KDE4_ADD_EXECUTABLE(testmagickiface ${testmagickiface_SRCS})

TARGET_LINK_LIBRARIES(testmagickiface
                      ${QT_QTTEST_LIBRARY}
                      ${QT_LIBRARIES}
                      ${ImageMagick_LIBRARIES}
                     )

ADD_TEST(NAME testmagickiface COMMAND testmagickiface)
