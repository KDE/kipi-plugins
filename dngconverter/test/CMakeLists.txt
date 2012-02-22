
# =======================================================
# DNGVALIDATE tool from DNG SDK

SET(dngvalidate_SRCS ../dngwriter/extra/dng_sdk/dng_validate.cpp)

KDE4_ADD_EXECUTABLE(dngvalidate NOGUI ${dngvalidate_SRCS})

TARGET_LINK_LIBRARIES(dngvalidate libdng ${CMAKE_THREAD_LIBS_INIT})

# =======================================================
# RAW2DNG command line tool

SET(raw2dng_SRCS raw2dng.cpp)

KDE4_ADD_EXECUTABLE(raw2dng NOGUI
                    ${libdngwriter_SRCS}
                    ${raw2dng_SRCS}
                   )

TARGET_LINK_LIBRARIES(raw2dng
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${QT_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      libdng
                      kipiplugins
                     )
