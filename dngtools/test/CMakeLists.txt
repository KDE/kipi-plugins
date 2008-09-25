
# =======================================================

# DNGVALIDATE tool from DNG SDK

# Disable DNG SDK warnings: we will never touch this code.
FOREACH(_curentfile ${libdng_SRCS})
    SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS -w)
ENDFOREACH(_curentfile ${libdng_SRCS})

# Disable XMP SDK warnings: we will never touch this code.
FOREACH(_curentfile ${libxmp_SRCS})
    SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS -w)
ENDFOREACH(_curentfile ${libxmp_SRCS})

ADD_DEFINITIONS(-DqDNGValidateTarget=1)

SET(dngvalidate_SRCS ../dngwriter/extra/dng_sdk/dng_validate.cpp)

KDE4_ADD_EXECUTABLE(dngvalidate NOGUI
                    ${libxmp_SRCS}
                    ${libmd5_SRCS}
                    ${libdng_SRCS}
                    ${dngvalidate_SRCS}
                   )

TARGET_LINK_LIBRARIES(dngvalidate 
                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                     )

# =======================================================
# RAW2DNG tool

SET(raw2dng_SRCS raw2dng.cpp)

KDE4_ADD_EXECUTABLE(raw2dng NOGUI
                    ${libdngwriter_SRCS}
                    ${raw2dng_SRCS}
                   )

TARGET_LINK_LIBRARIES(raw2dng
                      ${EXPAT_LIBRARY}
                      ${KDE4_KDEUI_LIBS}
                      ${QT_AND_KDECORE_LIBS} 
                      ${QT_LIBRARIES} 
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                     )
