CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/tools/kpversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/kpversion.h)

INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${TIFF_INCLUDE_DIR})

SET(kipiplugins_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpaboutdata.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpwriteimage.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpimageinfo.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kphostsettings.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpmetasettings.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpwritehelp.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpversion.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kprawthumbthread.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/binaryiface.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/iccjpeg.c
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/threads/actionthreadbase.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/threads/kpweaverobserver.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/batchprogressdialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/imagedialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/wizardpage.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/outputdialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/binarysearch.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/imageslist.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/savesettingswidget.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/previewmanager.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/previewimage.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/progresswidget.cpp
                         )

KDE4_ADD_LIBRARY(kipiplugins SHARED ${kipiplugins_LIB_SRCS})

TARGET_LINK_LIBRARIES(kipiplugins
                      ${QT_QTGUI_LIBRARY}
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBRARY}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KFILE_LIBS}
                      ${KDE4_SOLID_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${JPEG_LIBRARIES}
                      ${TIFF_LIBRARIES}
                      ${PNG_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                     )

SET_TARGET_PROPERTIES(kipiplugins PROPERTIES VERSION 2.0.0 SOVERSION 2)

INSTALL(TARGETS kipiplugins ${INSTALL_TARGETS_DEFAULT_ARGS})

INSTALL(PROGRAMS kipiplugins.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR})
