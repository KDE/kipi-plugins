SUBDIRS(pics)

SET(kipiplugin_rawconverter_PART_SRCS plugin_rawconverter.cpp 
                                      rawdecodingiface.cpp 
                                      savesettingswidget.cpp 
                                      previewwidget.cpp 
                                      actionthread.cpp 
                                      batchdialog.cpp 
                                      singledialog.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_rawconverter ${kipiplugin_rawconverter_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_rawconverter 
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      ${KDCRAW_LIBRARIES} 
                      ${QT_QT3SUPPORT_LIBRARY} 
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_rawconverter DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_rawconverter.desktop DESTINATION ${SERVICES_INSTALL_DIR})
