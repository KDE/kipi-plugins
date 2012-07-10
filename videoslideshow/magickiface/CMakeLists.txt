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

FOREACH(_currentfile ${magickiface_SRCS})
    IF(NOT MSVC)
        SET_SOURCE_FILES_PROPERTIES(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    ENDIF(NOT MSVC)
ENDFOREACH(_currentfile ${magickiface_SRCS})

KDE4_ADD_LIBRARY(magickiface STATIC ${magickiface_SRCS})

IF(NOT MSVC)
    SET_TARGET_PROPERTIES(magickiface PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF(NOT MSVC)

TARGET_LINK_LIBRARIES(magickiface
                      ${QT_QTGUI_LIBRARY}
                      ${KDE4_KDECORE_LIBS}
                      ${ImageMagick_LIBRARIES}
                     )
