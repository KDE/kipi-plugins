# Find HUpnp - HUpnp is a Universal Plug and Play (UPnP) library
# used by the UPnP collection.
#
# This module defines
# HUPNP_INCLUDE_DIR
# HUPNP_LIBS
# HUPNP_FOUND

FIND_PATH(HUPNP_INCLUDE_DIR
          HUpnp HINTS
          ${PC_HUPNP_INCLUDE_DIR}
          /usr/include/HUpnpCore/
          /usr/include/HUpnpAv/
         )

FIND_LIBRARY(HUPNP_LIBS HUpnp PATHS ${PC_HUPNP_LIBRARY})

IF(HUPNP_INCLUDE_DIR AND HUPNP_LIBS)
    SET(HUPNP_FOUND TRUE)
    MESSAGE(STATUS "Found HUpnp")
ELSE(HUPNP_INCLUDE_DIR and HUPNP_LIBS)
    SET(HUPNP_FOUND FALSE)
    IF(HUPNP_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could NOT find required package HUpnp: <http://herqq.org>")
    ENDIF(HUPNP_FIND_REQUIRED)
ENDIF(HUPNP_INCLUDE_DIR AND HUPNP_LIBS)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Hupnp DEFAULT_MSG HUPNP_INCLUDE_DIR HUPNP_LIBS)
