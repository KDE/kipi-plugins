
# =======================================================
# DNGINFO command line tool

SET(dnginfo_SRCS dnginfo.cpp)

KDE4_ADD_EXECUTABLE(dnginfo NOGUI ${dnginfo_SRCS})

TARGET_LINK_LIBRARIES(dnginfo
                      libdng
                      ${KDE4_KDECORE_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${QT_LIBRARIES}
                      ${EXPAT_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                     )

INSTALL(TARGETS dnginfo ${INSTALL_TARGETS_DEFAULT_ARGS})
