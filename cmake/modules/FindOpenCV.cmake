# ------------------------------------------------------------------------------
#
# - Try to find OpenCV library installation
# See http://sourceforge.net/projects/opencvlibrary
#
# The follwoing variables are optionally searched for defaults
#  OpenCV_ROOT_DIR:            Base directory of OpenCv tree to use.
#  OpenCV_FIND_REQUIRED_COMPONENTS : FIND_PACKAGE(OpenCV COMPONENTS ..) 
#    compatible interface. typically  CV CXCORE CVAUX HIGHGUI CVCAM .. etc.
#
# The following are set after configuration is done: 
#  OpenCV_FOUND
#  OpenCV_INCLUDE_DIRS
#  OpenCV_LIBRARIES
#  OpenCV_LINK_DIRECTORIES
#  OpenCV_ROOT_DIR
#  OpenCV_CV_INCLUDE_DIR
#  OpenCV_CXCORE_INCLUDE_DIR
#  OpenCV_CVAUX_INCLUDE_DIR
#  OpenCV_CVCAM_INCLUDE_DIR
#  OpenCV_HIGHGUI_INCLUDE_DIR
#  OpenCV_CV_LIBRARY
#  OpenCV_CXCORE_LIBRARY
#  OpenCV_CVAUX_LIBRARY
#  OpenCV_CVCAM_LIBRARY
#  OpenCV_CVHAARTRAINING_LIBRARY
#  OpenCV_CXTS_LIBRARY
#  OpenCV_HIGHGUI_LIBRARY
#  OpenCV_ML_LIBRARY
#  OpenCV_TRS_LIBRARY
#  OpenCV_BLOB_LIBRARY
#
# 2008/11 Caulier Gilles
# 2004/05 Jan Woetzel, Friso, Daniel Grest 
# 2006/01 complete rewrite by Jan Woetzel
# 1006/09 2nd rewrite introducing ROOT_DIR and PATH_SUFFIXES 
#         to handle multiple installed versions gracefully by Jan Woetzel
#
# tested with:
# -OpenCV 0.97 (beta5a): MSVS 7.1, gcc 3.3, gcc 4.1
# -OpenCV 0.99 (1.0rc1): MSVS 7.1
# -OpenCV 1.1  (pre1)  : gcc 3.5
#
# ------------------------------------------------------------------------------

MACRO(DBG_MSG _MSG)
    MESSAGE(STATUS "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}):\n${_MSG}")
ENDMACRO(DBG_MSG)

# Required OpenCV components with header and library if COMPONENTS unspecified
#
IF(NOT OpenCV_FIND_COMPONENTS)
    # Default components to find.
    SET(OpenCV_FIND_REQUIRED_COMPONENTS CV CXCORE CVAUX HIGHGUI)
ENDIF (NOT OpenCV_FIND_COMPONENTS)

# Typical root dirs of installations, exactly one of them is used
#
SET (OpenCV_POSSIBLE_ROOT_DIRS
     "${OpenCV_ROOT_DIR}"
     "$ENV{OpenCV_ROOT_DIR}"  
     "$ENV{OPENCV_DIR}"  # only for backward compatibility deprecated by ROOT_DIR
     "$ENV{OPENCV_HOME}" # only for backward compatibility
     "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Intel(R) Open Source Computer Vision Library_is1;Inno Setup: App Path]"
     "$ENV{ProgramFiles}/OpenCV/"
     "$ENV{ProgramFiles}/OpenCV2.0/"
     /usr/local/
     /usr/
    )

#DBG_MSG("DBG (OpenCV_POSSIBLE_ROOT_DIRS=${OpenCV_POSSIBLE_ROOT_DIRS}")

# Select exactly ONE OpenCV base directory/tree 
# to avoid mixing different version headers and libs
#
FIND_PATH(OpenCV_ROOT_DIR 
          NAMES 
          cv/include/cv.h     # windows
          include/opencv/cv.h # linux /opt/net
          include/cv/cv.h 
          include/cv.h 
          PATHS ${OpenCV_POSSIBLE_ROOT_DIRS}
         )

MESSAGE(STATUS "OpenCV root directory: ${OpenCV_ROOT_DIR}")

# Header include dir suffixes appended to OpenCV_ROOT_DIR
#
SET(OpenCV_INCDIR_SUFFIXES
    include
    include/cv
    include/opencv
    cv/include
    cxcore/include
    cvaux/include
    otherlibs/cvcam/include
    otherlibs/highgui
    otherlibs/highgui/include
    otherlibs/_graphics/include
   )

# Library link directory suffixes appended to OpenCV_ROOT_DIR 
#
SET(OpenCV_LIBDIR_SUFFIXES
    lib
    OpenCV/lib
    otherlibs/_graphics/lib
   )

#DBG_MSG("OpenCV_LIBDIR_SUFFIXES=${OpenCV_LIBDIR_SUFFIXES}")


# Find include directory for each lib
#
FIND_PATH(OpenCV_CV_INCLUDE_DIR
          NAMES cv.h
          PATHS ${OpenCV_ROOT_DIR}
          PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} 
         )

FIND_PATH(OpenCV_CXCORE_INCLUDE_DIR
          NAMES cxcore.h
          PATHS ${OpenCV_ROOT_DIR} 
          PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} 
         )

FIND_PATH(OpenCV_CVAUX_INCLUDE_DIR
          NAMES cvaux.h
          PATHS ${OpenCV_ROOT_DIR}
          PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} 
         )

FIND_PATH(OpenCV_HIGHGUI_INCLUDE_DIR
          NAMES highgui.h
          PATHS ${OpenCV_ROOT_DIR}
          PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} 
         )

FIND_PATH(OpenCV_CVCAM_INCLUDE_DIR
          NAMES cvcam.h
          PATHS ${OpenCV_ROOT_DIR}
          PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} 
         )

# Find asbsolute path to all libraries 
# some are optionally, some may not exist on Linux
#
FIND_LIBRARY(OpenCV_CV_LIBRARY
             NAMES cv cv200 opencv
             PATHS ${OpenCV_ROOT_DIR}
             PATH_SUFFIXES  ${OpenCV_LIBDIR_SUFFIXES} 
            )

FIND_LIBRARY(OpenCV_CVAUX_LIBRARY
             NAMES cvaux cvaux200
             PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} 
            )

FIND_LIBRARY(OpenCV_CVCAM_LIBRARY
             NAMES cvcam
             PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} 
            )

FIND_LIBRARY(OpenCV_CVHAARTRAINING_LIBRARY
             NAMES cvhaartraining
             PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} 
            )

FIND_LIBRARY(OpenCV_CXCORE_LIBRARY
             NAMES cxcore cxcore200
             PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} 
            )

FIND_LIBRARY(OpenCV_CXTS_LIBRARY
             NAMES cxts cxts200
             PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} 
            )

FIND_LIBRARY(OpenCV_HIGHGUI_LIBRARY
             NAMES highgui highgui200
             PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} 
            )

FIND_LIBRARY(OpenCV_ML_LIBRARY
             NAMES ml ml200
             PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} 
            )

FIND_LIBRARY(OpenCV_TRS_LIBRARY
             NAMES trs
             PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} 
            )

FIND_LIBRARY(OpenCV_BLOB_LIBRARY
             NAMES blob
             PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} 
            )

# Logic selecting required libs and headers
#
SET(OpenCV_FOUND ON)
#DBG_MSG("OpenCV_FIND_REQUIRED_COMPONENTS=${OpenCV_FIND_REQUIRED_COMPONENTS}")

FOREACH(NAME ${OpenCV_FIND_REQUIRED_COMPONENTS})

    # Only good if header and library both found   
    IF(OpenCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)

        LIST(APPEND OpenCV_INCLUDE_DIRS ${OpenCV_${NAME}_INCLUDE_DIR} )
        LIST(APPEND OpenCV_LIBRARIES    ${OpenCV_${NAME}_LIBRARY} )
        #DBG_MSG("appending for NAME=${NAME} ${OpenCV_${NAME}_INCLUDE_DIR} and ${OpenCV_${NAME}_LIBRARY}" )

    ELSE(OpenCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)

        MESSAGE(STATUS "OpenCV component ${NAME} not found!"
                "\nOpenCV_${NAME}_INCLUDE_DIR=${OpenCV_${NAME}_INCLUDE_DIR}"
                "\nOpenCV_${NAME}_LIBRARY=${OpenCV_${NAME}_LIBRARY}")
        SET(OpenCV_FOUND OFF)

    ENDIF(OpenCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)

ENDFOREACH(NAME)

MESSAGE(STATUS "OpenCV Include Directory: ${OpenCV_INCLUDE_DIRS}")
MESSAGE(STATUS "OpenCV Libraries: ${OpenCV_LIBRARIES}")

# Get the link directory for rpath to be used with LINK_DIRECTORIES: 
#
IF(OpenCV_CV_LIBRARY)
    GET_FILENAME_COMPONENT(OpenCV_LINK_DIRECTORIES ${OpenCV_CV_LIBRARY} PATH)
ENDIF(OpenCV_CV_LIBRARY)

MARK_AS_ADVANCED(OpenCV_ROOT_DIR
                 OpenCV_INCLUDE_DIRS
                 OpenCV_CV_INCLUDE_DIR
                 OpenCV_CXCORE_INCLUDE_DIR
                 OpenCV_CVAUX_INCLUDE_DIR
                 OpenCV_CVCAM_INCLUDE_DIR
                 OpenCV_HIGHGUI_INCLUDE_DIR
                 OpenCV_LIBRARIES
                 OpenCV_CV_LIBRARY
                 OpenCV_CXCORE_LIBRARY
                 OpenCV_CVAUX_LIBRARY
                 OpenCV_CVCAM_LIBRARY
                 OpenCV_CVHAARTRAINING_LIBRARY
                 OpenCV_CXTS_LIBRARY
                 OpenCV_HIGHGUI_LIBRARY
                 OpenCV_ML_LIBRARY
                 OpenCV_TRS_LIBRARY
                 OpenCV_BLOB_LIBRARY
                )

# Be backward compatible:
SET(OPENCV_LIBRARIES   ${OpenCV_LIBRARIES})
SET(OPENCV_INCLUDE_DIR ${OpenCV_INCLUDE_DIRS})
SET(OPENCV_FOUND       ${OpenCV_FOUND})

# Display help message
IF(NOT OpenCV_FOUND)

    # Make FIND_PACKAGE friendly
    IF(NOT OpenCV_FIND_QUIETLY)

        IF(OpenCV_FIND_REQUIRED)
            MESSAGE(FATAL_ERROR "Error: OpenCV required but some headers or libs not found. "
                                "Please specify it's location with OpenCV_ROOT_DIR env. variable.")
        ELSE(OpenCV_FIND_REQUIRED)
            MESSAGE(STATUS "Warning: OpenCV was not found.")
        ENDIF(OpenCV_FIND_REQUIRED)

    ENDIF(NOT OpenCV_FIND_QUIETLY)

ENDIF(NOT OpenCV_FOUND)
