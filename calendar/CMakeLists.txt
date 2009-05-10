SET(kipiplugin_calendar_PART_SRCS plugin_calendar.cpp
                                  calpainter.cpp
                                  calprinter.cpp
                                  calselect.cpp
                                  calsettings.cpp
                                  caltemplate.cpp
                                  calwidget.cpp
                                  calwizard.cpp
                                  monthwidget.cpp
   )

KDE4_ADD_UI_FILES(kipiplugin_calendar_PART_SRCS caltemplate.ui
                                                calevents.ui
                                                calselect.ui
                                                calprogress.ui
                 )

KDE4_ADD_PLUGIN(kipiplugin_calendar ${kipiplugin_calendar_PART_SRCS})

INCLUDE_DIRECTORIES(${KDEPIMLIBS_INCLUDE_DIR})

TARGET_LINK_LIBRARIES(kipiplugin_calendar 
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      ${KDCRAW_LIBRARIES} 
                      ${KDE4_KCAL_LIBS} 
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_calendar DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_calendar.desktop DESTINATION ${SERVICES_INSTALL_DIR})
