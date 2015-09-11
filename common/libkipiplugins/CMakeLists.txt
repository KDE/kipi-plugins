#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/kpversion.h.cmake.in              ${CMAKE_CURRENT_BINARY_DIR}/kpversion.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/templates/Doxyfile.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

include_directories(${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${TIFF_INCLUDE_DIR})

set(kipiplugins_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/tools/kipiplugins_debug.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kputil.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpimageinfo.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpmetadata.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kphostsettings.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpmetasettings.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kprawthumbthread.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpbinaryiface.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpversion.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpaboutdata.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/imageio/iccjpeg.c
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/imageio/kpwritehelp.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/imageio/kpwriteimage.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpbinarysearch.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpprogresswidget.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kppreviewimage.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kppreviewmanager.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpsavesettingswidget.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpimageslist.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpsettingswidget.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpwizardpage.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpimagedialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpbatchprogressdialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kptooldialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpoutputdialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpnewalbumdialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kplogindialog.cpp
)

add_library(KF5kipiplugins SHARED ${kipiplugins_LIB_SRCS})

generate_export_header(KF5kipiplugins BASE_NAME kipiplugins EXPORT_MACRO_NAME KIPIPLUGINS_EXPORT)

target_link_libraries(KF5kipiplugins

                      PRIVATE
                      ${JPEG_LIBRARIES}
                      ${TIFF_LIBRARIES}
                      ${PNG_LIBRARIES}

                      PUBLIC
                      Qt5::Gui
                      KF5::I18n
                      KF5::ConfigCore
                      KF5::KIOFileWidgets
                      KF5::WidgetsAddons
                      KF5::CoreAddons

                      ${KF5Kipi_LIBRARIES}
                      ${KF5KExiv2_LIBRARIES}
                      ${KF5KDcraw_LIBRARIES}
)

set_target_properties(KF5kipiplugins PROPERTIES
                      VERSION     ${KIPIPLUGINS_LIB_SO_VERSION_STRING}
                      SOVERSION   ${KIPIPLUGINS_LIB_SO_VERSION_STRING}
                      EXPORT_NAME KIPIPlugins
)

install(TARGETS KF5kipiplugins
        EXPORT  KF5kipipluginsTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)
