SET(kipiplugin_metadataedit_PART_SRCS plugin_metadataedit.cpp 
                                      metadatacheckbox.cpp
                                      multistringsedit.cpp
                                      multivalstringsedit.cpp
                                      multivaluesedit.cpp
                                      exifcaption.cpp
                                      exifdatetime.cpp
                                      exifadjust.cpp
                                      exiflens.cpp 
                                      exifdevice.cpp
                                      exiflight.cpp
                                      exifeditdialog.cpp
                                      iptccontent.cpp
                                      iptcsubjects.cpp
                                      iptckeywords.cpp
                                      iptccategories.cpp
                                      iptccredits.cpp
                                      iptcdatetime.cpp
                                      iptcstatus.cpp
                                      iptcorigin.cpp
                                      iptceditdialog.cpp 
                                      commenteditdialog.cpp
                                      commentremovedialog.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_metadataedit ${kipiplugin_metadataedit_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_metadataedit 
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      kipiplugins
                     )

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

INSTALL(TARGETS kipiplugin_metadataedit DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_metadataedit.desktop DESTINATION ${SERVICES_INSTALL_DIR})
