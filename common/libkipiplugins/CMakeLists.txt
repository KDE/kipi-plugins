#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/kpversion.h.cmake.in              ${CMAKE_CURRENT_BINARY_DIR}/kpversion.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/templates/Doxyfile.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

include_directories(${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${TIFF_INCLUDE_DIR})

set(kipiplugins_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpaboutdata.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpmetadata.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpimageinfo.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kphostsettings.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpmetasettings.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpversion.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kprawthumbthread.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpbinaryiface.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/imageio/kpwritehelp.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/imageio/kpwriteimage.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/imageio/iccjpeg.c
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kptooldialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpbatchprogressdialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpimagedialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpwizardpage.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpoutputdialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpbinarysearch.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpprogresswidget.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpimageslist.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kppreviewimage.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kppreviewmanager.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpsavesettingswidget.cpp
  )

KDE4_ADD_LIBRARY(kipiplugins SHARED ${kipiplugins_LIB_SRCS})

target_link_libraries(kipiplugins
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

set_target_properties(kipiplugins PROPERTIES VERSION ${KIPIPLUGINS_LIB_SO_VERSION_STRING} SOVERSION ${KIPIPLUGINS_LIB_SO_VERSION_STRING})

install(TARGETS kipiplugins ${INSTALL_TARGETS_DEFAULT_ARGS})
