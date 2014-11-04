#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${QJSON_INCLUDE_DIR})

set(kipiplugin_imgurexport_PART_SRCS
    plugin_imgurexport.cpp
    imgurtalker.cpp
    imgurwidget.cpp
    imgurwindow.cpp
    imgurimageslist.cpp
    mpform.cpp
   )

if(QTKOAUTH_FOUND)
    include_directories(${QTKOAUTH_INCLUDE_DIR})
    add_definitions(-DOAUTH_ENABLED
                    ${QTKOAUTH_DEFINITIONS}
                   )

    set(kipiplugin_imgurexport_PART_SRCS
        ${kipiplugin_imgurexport_PART_SRCS}
        imgurtalkerauth.cpp
       )
endif()

KDE4_ADD_PLUGIN(kipiplugin_imgurexport ${kipiplugin_imgurexport_PART_SRCS})

target_link_libraries(kipiplugin_imgurexport
                      ${KIPI_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KEXIV2_LIBRARIES}
                      ${QJSON_LIBRARIES}
                      kipiplugins
                     )

if(QTKOAUTH_FOUND)
    target_link_libraries(kipiplugin_imgurexport
                          ${QTKOAUTH_LIBRARIES}
  )
endif()

configure_file(kipiplugin_imgurexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imgurexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imgurexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_imgurexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_imgurexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
