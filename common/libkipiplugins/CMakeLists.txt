CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pluginsversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/pluginsversion.h)

SET(kipiplugins_LIB_SRCS kpaboutdata.cpp)

KDE4_ADD_LIBRARY(kipiplugins SHARED ${kipiplugins_LIB_SRCS})

TARGET_LINK_LIBRARIES(kipiplugins
                      ${QT_QTGUI_LIBRARY}
                      ${KDE4_KDECORE_LIBS} 
                     )

SET_TARGET_PROPERTIES(kipiplugins PROPERTIES VERSION 1.0.0 SOVERSION 1 )

INSTALL(TARGETS kipiplugins DESTINATION ${LIB_INSTALL_DIR} )
