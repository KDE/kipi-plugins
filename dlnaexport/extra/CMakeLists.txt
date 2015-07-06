#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

#add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
#add_definitions(-DQT_NO_CAST_FROM_ASCII)
#add_definitions(-DQT_NO_CAST_TO_ASCII)

# TODO : remove this directive step by step to port to pure Qt5.
remove_definitions(-DQT_STRICT_ITERATORS)
remove_definitions(-DQT_NO_URL_CAST_FROM_STRING)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
remove_definitions(-DQT_USE_FAST_OPERATOR_PLUS)
remove_definitions(-DQT_USE_QSTRINGBUILDER)

# Directive to compile Hupnp internal components.
add_definitions(-DH_BUILD_UPNP_CORE_LIB)
add_definitions(-DH_BUILD_UPNP_LIGHTING_LIB)
add_definitions(-DH_BUILD_UPNP_AV_LIB)

add_subdirectory(qtsoap)
add_subdirectory(hupnp)
add_subdirectory(hupnp_av)

if(BUILD_TESTING)
    add_subdirectory(avtest)
endif()
