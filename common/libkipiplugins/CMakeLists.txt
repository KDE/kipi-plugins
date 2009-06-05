CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pluginsversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/pluginsversion.h)
INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${TIFF_INCLUDE_DIR})

SET(kipiplugins_LIB_SRCS kpaboutdata.cpp
                         kpwriteimage.cpp
                         iccjpeg.c
                         batchprogressdialog.cpp
                         imageslist.cpp
                         imagedialog.cpp
                         )

KDE4_ADD_LIBRARY(kipiplugins SHARED ${kipiplugins_LIB_SRCS})

TARGET_LINK_LIBRARIES(kipiplugins
                      ${QT_QTGUI_LIBRARY}
                      ${KDE4_KDECORE_LIBS} 
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBRARY}
                      ${KDE4_KIO_LIBS} 
                      ${JPEG_LIBRARIES}
                      ${TIFF_LIBRARIES}
                      ${PNG_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES} 
                      ${KIPI_LIBRARIES} 
                     )

SET_TARGET_PROPERTIES(kipiplugins PROPERTIES VERSION 1.0.0 SOVERSION 1 )

INSTALL(TARGETS kipiplugins ${INSTALL_TARGETS_DEFAULT_ARGS} )

install( FILES kipiplugins.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
