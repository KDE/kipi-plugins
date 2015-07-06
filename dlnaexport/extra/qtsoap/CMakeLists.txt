
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

#INSTALL(TARGETS QtSoap ARCHIVE DESTINATION lib ${INSTALL_TARGETS_DEFAULT_ARGS})
