SET(kipiplugin_calendar_PART_SRCS plugin/plugin_calendar.cpp
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

INCLUDE_DIRECTORIES(${KDEPIMLIBS_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${CMAKE_CURRENT_SOURCE_DIR}/wizard
                    ${CMAKE_CURRENT_SOURCE_DIR}/print
                   )

TARGET_LINK_LIBRARIES(kipiplugin_calendar 
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      ${KDCRAW_LIBRARIES} 
                      ${KDE4_KCALCORE_LIBS} 
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_calendar DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES plugin/kipiplugin_calendar.desktop DESTINATION ${SERVICES_INSTALL_DIR})
