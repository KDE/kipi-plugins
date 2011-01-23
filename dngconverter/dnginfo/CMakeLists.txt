
# =======================================================
# DNGINFO command line tool

# Disable DNG SDK warnings: we will never touch this code.
FOREACH(_currentfile ${libdng_SRCS})
    SET_SOURCE_FILES_PROPERTIES(${_currentfile} PROPERTIES COMPILE_FLAGS -w)
ENDFOREACH(_currentfile ${libdng_SRCS})

# Disable XMP SDK warnings: we will never touch this code.
FOREACH(_currentfile ${libxmp_SRCS})
    SET_SOURCE_FILES_PROPERTIES(${_currentfile} PROPERTIES COMPILE_FLAGS -w)
ENDFOREACH(_currentfile ${libxmp_SRCS})

SET(dnginfo_SRCS dnginfo.cpp)

KDE4_ADD_EXECUTABLE(dnginfo NOGUI
                    ${libxmp_SRCS}
                    ${libmd5_SRCS}
                    ${libdng_SRCS}
                    ${dnginfo_SRCS}
                   )

TARGET_LINK_LIBRARIES(dnginfo
                      ${KDE4_KDECORE_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${QT_LIBRARIES}
                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                     )

INSTALL(TARGETS dnginfo ${INSTALL_TARGETS_DEFAULT_ARGS})