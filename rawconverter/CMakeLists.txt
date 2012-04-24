SUBDIRS(icons)

INCLUDE_DIRECTORIES(
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
                    ${JPEG_INCLUDE_DIR}
                    ${PNG_PNG_INCLUDE_DIR}
                   )

SET(kipiplugin_rawconverter_PART_SRCS plugin/plugin_rawconverter.cpp
                                      manager/rawdecodingiface.cpp
                                      manager/actionthread.cpp
                                      dialogs/batchdialog.cpp
                                      dialogs/singledialog.cpp
                                      dialogs/myimagelist.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_rawconverter ${kipiplugin_rawconverter_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_rawconverter
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_rawconverter DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES plugin/kipiplugin_rawconverter.desktop DESTINATION ${SERVICES_INSTALL_DIR})
