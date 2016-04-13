#
# Copyright (c) 2010-2016, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/kpversion.h.cmake.in              ${CMAKE_CURRENT_BINARY_DIR}/kpversion.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/templates/Doxyfile.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

include_directories(${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${TIFF_INCLUDE_DIR})

qt5_add_resources(libkipipluginsresources_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../data/libkipiplugins.qrc)

set(kipiplugins_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/tools/kipiplugins_debug.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kputil.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpimageinfo.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpversion.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpaboutdata.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/tools/kpthreadmanager.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpprogresswidget.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpsavesettingswidget.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpimageslist.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/widgets/kpsettingswidget.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpwizardpage.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpimagedialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpbatchprogressdialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kptooldialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kpnewalbumdialog.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/kplogindialog.cpp

                         ${libkipipluginsresources_SRCS}
)

add_library(KF5kipiplugins SHARED ${kipiplugins_LIB_SRCS})
add_dependencies(KF5kipiplugins kipiplugins-gitversion)

generate_export_header(KF5kipiplugins BASE_NAME kipiplugins EXPORT_MACRO_NAME KIPIPLUGINS_EXPORT)

target_link_libraries(KF5kipiplugins

                      PUBLIC
                      Qt5::Gui

                      PRIVATE

                      KF5::I18n
                      KF5::ConfigCore

                      KF5::Kipi
)

set_target_properties(KF5kipiplugins PROPERTIES
                      VERSION     ${KIPIPLUGINS_LIB_SO_VERSION_STRING}
                      SOVERSION   ${KIPIPLUGINS_LIB_SO_VERSION_STRING}
                      EXPORT_NAME KIPIPlugins
)

install(TARGETS KF5kipiplugins
        EXPORT  KF5kipipluginsTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)
