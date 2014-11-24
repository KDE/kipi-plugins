#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_subdirectory(icons)
add_subdirectory(data)

if(HUPNP_FOUND AND HUPNPAV_FOUND)

    include_directories(${HUPNP_INCLUDE_DIR})

elseif(NOT (HUPNP_FOUND AND HUPNPAV_FOUND) AND QTSOAP_FOUND)

    include_directories(extra/hupnp/include)
    include_directories(extra/hupnp_av/include)

    add_definitions(-DH_BUILD_UPNP_CORE_LIB)
    add_definitions(-DH_BUILD_UPNP_LIGHTING_LIB)
    add_definitions(-DH_BUILD_UPNP_AV_LIB)

    add_subdirectory(extra)

endif()

set(kipiplugin_dlnaexport_PART_SRCS
    finalpage.cpp
    plugin_dlnaexport.cpp
    hupnpmediaserver.cpp
    wizard.cpp
    welcomepage.cpp
    minidlnamediaserver.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_dlnaexport ${kipiplugin_dlnaexport_PART_SRCS})

target_link_libraries(kipiplugin_dlnaexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_LIBRARIES}
                      ${QT_QTXMLPATTERNS_LIBRARY}
                      kipiplugins
                     )

if(HUPNP_FOUND AND HUPNPAV_FOUND)

    target_link_libraries(kipiplugin_dlnaexport
                          ${HUPNP_LIBS}
                          ${HUPNPAV_LIBS}
                         )

elseif(NOT (HUPNP_FOUND AND HUPNPAV_FOUND) AND QTSOAP_FOUND)

    target_link_libraries(kipiplugin_dlnaexport
                          HUpnp
                          HUpnpAv
                         )

endif()

configure_file(kipiplugin_dlnaexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dlnaexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dlnaexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_dlnaexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_dlnaexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
