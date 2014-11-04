#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/src
                    src
                    ${QT_INCLUDE_DIR}
                   )

set(QtSoap_SRCS src/qtsoap.cpp)

QT4_WRAP_CPP(QtSoap_MOC_SRCS src/qtsoap.h)

set(SRCS ${QtSoap_MOC_SRCS} ${QtSoap_SRCS})

foreach(_currentfile ${SRCS})
    if(NOT MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
endforeach(_currentfile ${SRCS})

add_library(QtSoap STATIC ${SRCS})

IF(NOT MSVC)
    set_target_properties(QtSoap PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

target_link_libraries(QtSoap ${QT_LIBRARIES})

#install(TARGETS QtSoap ARCHIVE DESTINATION lib ${INSTALL_TARGETS_DEFAULT_ARGS})
