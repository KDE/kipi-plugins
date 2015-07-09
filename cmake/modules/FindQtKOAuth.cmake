# - Try to find the kQOAuth library
# Once done this will define
#
#  QTKOAUTH_FOUND - system has the kQOAuth library
#  QTKOAUTH_INCLUDE_DIR - the kQOAuth include directory
#  QTKOAUTH_LIBRARIES - Link this to use the kQOAuth
#  QTKOAUTH_DEFINITIONS - Compiler switches required for using kQOAuth
#

if (QTKOAUTH_INCLUDE_DIR AND QTKOAUTH_LIBRARIES)
#   in cache already
  set(QTKOAUTH_FOUND TRUE)
else (QTKOAUTH_INCLUDE_DIR AND QTKOAUTH_LIBRARIES)
  if (NOT WIN32)
    find_package(PkgConfig)
    pkg_check_modules(PC_QTKOAUTH QUIET kqoauth)
    set(QTKOAUTH_DEFINITIONS ${PC_QTKOAUTH_CFLAGS_OTHER})
  endif(NOT WIN32)

  find_library(QTKOAUTH_LIBRARIES NAMES kqoauth kqoauth0
    HINTS ${PC_QTKOAUTH_LIBDIR} ${PC_QTKOAUTH_LIBRARIES_DIRS}
  )

  find_path(QTKOAUTH_INCLUDE_DIR kqoauthmanager.h
    HINTS ${PC_QTKOAUTH_INCLUDEDIR} ${PC_QTKOAUTH_INCLUDE_DIRS}
    PATH_SUFFIXES QtKOAuth
  )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(QtKOAuth DEFAULT_MSG QTKOAUTH_LIBRARIES QTKOAUTH_INCLUDE_DIR)

  mark_as_advanced(QTKOAUTH_INCLUDE_DIR QTKOAUTH_LIBRARIES)
endif (QTKOAUTH_INCLUDE_DIR AND QTKOAUTH_LIBRARIES)
