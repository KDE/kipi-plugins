# Magnus Homann svn hack
# Required?
SET(CMAKE_BACKWARDS_COMPATIBILITY "2.4")

# See if we have svn installed
FIND_PROGRAM(SVNVERSION svnversion)

# Read the version if installed, else set to "unknown"
IF (SVNVERSION)
        EXEC_PROGRAM(${SVNVERSION} ARGS /home/gilles/Documents/devel/graphics/kipi-plugins OUTPUT_VARIABLE MYVERSION)
ELSE (SVNVERSION)
        SET(MYVERSION unknown)
ENDIF (SVNVERSION)

# Configure the svnversion.h
CONFIGURE_FILE("/home/gilles/Documents/devel/graphics/kipi-plugins/cmake/templates/svnversion.h.in_cmake"
               "/home/gilles/Documents/devel/graphics/kipi-plugins/digikam/svnversion.h")
