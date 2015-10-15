#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2011-2015, Benjamin Girault, <benjamin dot girault at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(BUILD_TESTING)
    set(pto_parser_test_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/parser_test.cpp)

    add_executable(pto_parser_test ${pto_parser_test_SRCS})

    target_link_libraries(pto_parser_test libpanorama)
endif()
