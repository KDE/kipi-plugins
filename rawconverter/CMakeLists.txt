SUBDIRS(icons)

INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR} ${PNG_PNG_INCLUDE_DIR})

SET(kipiplugin_rawconverter_PART_SRCS plugin_rawconverter.cpp
                                      previewwidget.cpp
                                      savesettingswidget.cpp
                                      rawdecodingiface.cpp
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
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_rawconverter DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_rawconverter.desktop DESTINATION ${SERVICES_INSTALL_DIR})
