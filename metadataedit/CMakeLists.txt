SET(kipiplugin_metadataedit_PART_SRCS metadatacheckbox.cpp
                                      exifcaption.cpp
                                      exifdatetime.cpp
                                      exifadjust.cpp
                                      exiflens.cpp 
                                      exifdevice.cpp
                                      exiflight.cpp
                                      exifeditdialog.cpp
#                                       iptccaption.cpp
#                                       iptcsubjects.cpp
#                                       iptccredits.cpp
#                                       iptcstatus.cpp
#                                       iptcorigin.cpp
#                                       iptcdatetime.cpp
#                                       iptckeywords.cpp
#                                       iptccategories.cpp
#                                       iptceditdialog.cpp 
#                                       commenteditdialog.cpp
#                                       commentremovedialog.cpp
                                      plugin_metadataedit.cpp 
   )

KDE4_ADD_PLUGIN(kipiplugin_metadataedit ${kipiplugin_metadataedit_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_metadataedit 
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      ${QT_QT3SUPPORT_LIBRARY}
                      kipiplugins
                     )

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

INSTALL(TARGETS kipiplugin_metadataedit DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_metadataedit.desktop DESTINATION ${SERVICES_INSTALL_DIR})
