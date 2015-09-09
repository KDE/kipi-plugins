#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                   )

set(QtSoap_SRCS qtsoap.cpp)

qt5_wrap_cpp(QtSoap_MOC_SRCS qtsoap.h)

set(SRCS ${QtSoap_MOC_SRCS} ${QtSoap_SRCS})

foreach(_currentfile ${SRCS})
    if(NOT MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
endforeach()

add_library(QtSoap STATIC ${SRCS})

if(NOT MSVC)
    set_target_properties(QtSoap PROPERTIES COMPILE_FLAGS "-fPIC")
endif(NOT MSVC)

target_link_libraries(QtSoap Qt5::Xml Qt5::Network)
