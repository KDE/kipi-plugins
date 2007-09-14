# - Try to find the KSane library
# Once done this will define
#
#  KSANE_FOUND - system has libksane
#  KSANE_INCLUDE_DIR - the libksane include directory
#  KSANE_LIBRARIES - Link these to use libksane
#  KSANE_DEFINITIONS - Compiler switches required for using libksane
#

if (KSANE_INCLUDE_DIR AND KSANE_LIBRARIES)

  message(STATUS "Found KSane library in cache: ${KSANE_LIBRARIES}")

  # in cache already
  SET(KSANE_FOUND TRUE)

else (KSANE_INCLUDE_DIR AND KSANE_LIBRARIES)

  message(STATUS "Check KSane library in local sub-folder...")

  # Check if library is not in local sub-folder

  FIND_FILE(KSANE_LOCAL_FOUND libksane/version.h.cmake ${CMAKE_SOURCE_DIR}/libksane NO_DEFAULT_PATH)

  if (KSANE_LOCAL_FOUND)

    set(KSANE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libksane)
    set(KSANE_DEFINITIONS -I${KSANE_INCLUDE_DIR})
    set(KSANE_LIBRARIES ksane)
    message(STATUS "Found KSane library in local sub-folder: ${KSANE_LIBRARIES}")
    set(KSANE_FOUND TRUE)
    MARK_AS_ADVANCED(KSANE_INCLUDE_DIR KSANE_LIBRARIES)

  else(KSANE_LOCAL_FOUND)

    if(NOT WIN32) 
      message(STATUS "Check KSane library using pkg-config...")

      # use pkg-config to get the directories and then use these values
      # in the FIND_PATH() and FIND_LIBRARY() calls
      INCLUDE(UsePkgConfig)

      PKGCONFIG(libksane _KSANEIncDir _KSANELinkDir _KSANELinkFlags _KSANECflags)

      if(_KSANELinkFlags)
        # query pkg-config asking for a libksane >= 0.1.0
        EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=0.1.0 libksane RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )
        if(_return_VALUE STREQUAL "0")
            message(STATUS "Found libksane release >= 0.1.0")
            set(KSANE_VERSION_GOOD_FOUND TRUE)
        else(_return_VALUE STREQUAL "0")
            message(STATUS "Found libksane release < 0.1.0, too old")
            set(KSANE_VERSION_GOOD_FOUND FALSE)
            set(KSANE_FOUND FALSE)
        endif(_return_VALUE STREQUAL "0")
      else(_KSANELinkFlags)
        set(KSANE_VERSION_GOOD_FOUND FALSE)
        set(KSANE_FOUND FALSE)
      endif(_KSANELinkFlags)
    else(NOT WIN32)
    set(KSANE_VERSION_GOOD_FOUND TRUE)
    endif(NOT WIN32)
    if(KSANE_VERSION_GOOD_FOUND)
        set(KSANE_DEFINITIONS ${_KSANECflags})

        FIND_PATH(KSANE_INCLUDE_DIR libksane/version.h
        ${_KSANEIncDir}
        )
    
        FIND_LIBRARY(KSANE_LIBRARIES NAMES ksane
        PATHS
        ${_KSANELinkDir}
        )
    
        if (KSANE_INCLUDE_DIR AND KSANE_LIBRARIES)
            set(KSANE_FOUND TRUE)
        endif (KSANE_INCLUDE_DIR AND KSANE_LIBRARIES)
      endif(KSANE_VERSION_GOOD_FOUND)
      if (KSANE_FOUND)
          if (NOT KSane_FIND_QUIETLY)
              message(STATUS "Found libksane: ${KSANE_LIBRARIES}")
          endif (NOT KSane_FIND_QUIETLY)
      else (KSANE_FOUND)
          if (KSane_FIND_REQUIRED)
              if (NOT KSANE_INCLUDE_DIR)
                  message(FATAL_ERROR "Could NOT find libksane header files")
              endif (NOT KSANE_INCLUDE_DIR)
              if (NOT KSANE_LIBRARIES)
                  message(FATAL_ERROR "Could NOT find libksane library")
              endif (NOT KSANE_LIBRARIES)
          endif (KSane_FIND_REQUIRED)
      endif (KSANE_FOUND)

    MARK_AS_ADVANCED(KSANE_INCLUDE_DIR KSANE_LIBRARIES)

  endif(KSANE_LOCAL_FOUND)

endif (KSANE_INCLUDE_DIR AND KSANE_LIBRARIES)
