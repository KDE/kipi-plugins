#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_subdirectory(test)

include_directories(${ImageMagick_INCLUDE_DIRS}
                    ${QT_INCLUDES}
                    ${KDE4_INCLUDES}
                    ${CMAKE_BINARY_DIR}
                   )

set(magickiface_SRCS
    magickiface.cpp
    processimage.cpp
   )

foreach(_currentfile ${magickiface_SRCS})
    if(NOT MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
endforeach()

KDE4_ADD_LIBRARY(magickiface STATIC ${magickiface_SRCS})

if(NOT MSVC)
    set_target_properties(magickiface PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

target_link_libraries(magickiface
                      ${QT_QTGUI_LIBRARY}
                      ${KDE4_KDECORE_LIBS}
                      ${ImageMagick_LIBRARIES}
                     )
