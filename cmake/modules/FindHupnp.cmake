# - Find HUpnp
# HUpnp is a Universal Plug and Play (UPnP) library
# used by the UPnP collection.
# Defines:
# HUPNP_INCLUDE_DIR
# HUPNP_LIBS
# HUPNP_FOUND

find_path(HUPNP_INCLUDE_DIR
	HUpnp HINTS
	${PC_HUPNP_INCLUDE_DIR}
	/usr/include/HUpnpCore/
	/usr/include/HUpnpAv/
)

find_library(HUPNP_LIBS HUpnp PATHS ${PC_HUPNP_LIBRARY})

if(HUPNP_INCLUDE_DIR AND HUPNP_LIBS)
  set(HUPNP_FOUND TRUE)
  message(STATUS "Found HUpnp")
else(HUPNP_INCLUDE_DIR and HUPNP_LIBS)
  set(HUPNP_FOUND FALSE)
  if(HUPNP_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find required package HUpnp: <http://herqq.org>")
  endif(HUPNP_FIND_REQUIRED)
endif(HUPNP_INCLUDE_DIR AND HUPNP_LIBS)

include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(Hupnp DEFAULT_MSG HUPNP_INCLUDE_DIR HUPNP_LIBS )