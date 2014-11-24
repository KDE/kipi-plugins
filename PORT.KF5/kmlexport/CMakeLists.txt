#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(kipiplugin_kmlexport_PART_SRCS plugin_kmlexport.cpp
                                   gpsdataparser.cpp
                                   kmlexport.cpp
                                   kmlgpsdataparser.cpp
                                   kmlexportconfig.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_kmlexport ${kipiplugin_kmlexport_PART_SRCS})

target_link_libraries(kipiplugin_kmlexport
                      ${KDE4_KPARTS_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      kipiplugins
                     )

configure_file(kipiplugin_kmlexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_kmlexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_kmlexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_kmlexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_kmlexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)

if(KDE4_BUILD_TESTS)
    MACRO_OPTIONAL_ADD_SUBDIRECTORY(tests)
endif()
