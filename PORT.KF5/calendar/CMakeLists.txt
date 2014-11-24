#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(kipiplugin_calendar_PART_SRCS plugin/plugin_calendar.cpp
                                  print/calpainter.cpp
                                  print/calprinter.cpp
                                  print/calsettings.cpp
                                  wizard/caltemplate.cpp
                                  wizard/calwizard.cpp
                                  wizard/calwidget.cpp
                                  wizard/monthwidget.cpp
   )

KDE4_ADD_UI_FILES(kipiplugin_calendar_PART_SRCS ui/caltemplate.ui
                                                ui/calevents.ui
                                                ui/calprogress.ui
                 )

KDE4_ADD_PLUGIN(kipiplugin_calendar ${kipiplugin_calendar_PART_SRCS})

include_directories(${KDEPIMLIBS_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${CMAKE_CURRENT_SOURCE_DIR}/wizard
                    ${CMAKE_CURRENT_SOURCE_DIR}/print
                   )

target_link_libraries(kipiplugin_calendar 
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      ${KDCRAW_LIBRARIES} 
                      ${KDE4_KCALCORE_LIBS} 
                      kipiplugins
                     )

configure_file(plugin/kipiplugin_calendar.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_calendar.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_calendar.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_calendar                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_calendarui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
