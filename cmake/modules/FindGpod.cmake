# - Try to find Gpod
# Once done this will define
#
#  GPOD_FOUND - system has Gpod
#  GPOD_INCLUDE_DIR - the Gpod include directory
#  GPOD_LIBRARIES - Link these to use Gpod
#  GPOD_DEFINITIONS - Compiler switches required for using Gpod
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if ( GPOD_INCLUDE_DIR AND GPOD_LIBRARIES )
   # in cache already
   SET(Gpod_FIND_QUIETLY TRUE)
endif ( GPOD_INCLUDE_DIR AND GPOD_LIBRARIES )

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if( NOT WIN32 )
  find_package(PkgConfig)

  pkg_check_modules(PC_GPOD libgpod-1.0)

  set(GPOD_DEFINITIONS ${PC_GPOD_CFLAGS_OTHER})
  
  exec_program(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=0.7.0 libgpod-1.0 OUTPUT_VARIABLE _pkgconfigDevNull RETURN_VALUE GPOD_VERSION_OKAY)

endif( NOT WIN32 )

if (GPOD_VERSION_OKAY STREQUAL "0")

  FIND_PATH(GPOD_INCLUDE_DIR NAMES gpod/itdb.h
    PATHS
    ${PC_GPOD_INCLUDEDIR}
    ${PC_GPOD_INCLUDE_DIRS}
    PATH_SUFFIXES gpod-1.0
  )
  
  FIND_LIBRARY(GPOD_LIBRARIES NAMES gpod
    PATHS
    ${PC_GPOD_LIBDIR}
    ${PC_GPOD_LIBRARY_DIRS}
  )
  
  include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(Gpod DEFAULT_MSG GPOD_INCLUDE_DIR GPOD_LIBRARIES )
  
  # show the GPOD_INCLUDE_DIR and GPOD_LIBRARIES variables only in the advanced view
  MARK_AS_ADVANCED(GPOD_INCLUDE_DIR GPOD_LIBRARIES )

else (GPOD_VERSION_OKAY STREQUAL "0")
    set(GPOD_FOUND FALSE)
endif (GPOD_VERSION_OKAY STREQUAL "0")
