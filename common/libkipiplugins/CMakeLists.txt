CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pluginsversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/pluginsversion.h)
INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${TIFF_INCLUDE_DIR})

SET(kipiplugins_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpaboutdata.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpwriteimage.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/binaryiface.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/iccjpeg.c
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpwritehelp.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/batchprogressdialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/imagedialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/wizardpage.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/outputdialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/imageslist.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/savesettingswidget.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/previewmanager.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/previewimage.cpp
                         )

KDE4_ADD_LIBRARY(kipiplugins SHARED ${kipiplugins_LIB_SRCS})

TARGET_LINK_LIBRARIES(kipiplugins
                      ${QT_QTGUI_LIBRARY}
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBRARY}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KFILE_LIBS}
                      ${JPEG_LIBRARIES}
                      ${TIFF_LIBRARIES}
                      ${PNG_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                     )

SET_TARGET_PROPERTIES(kipiplugins PROPERTIES VERSION 1.0.0 SOVERSION 1)

INSTALL(TARGETS kipiplugins ${INSTALL_TARGETS_DEFAULT_ARGS})

install( PROGRAMS kipiplugins.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR})
