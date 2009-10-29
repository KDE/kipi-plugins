SUBDIRS(icons)

SET(kipiplugin_metadataedit_PART_SRCS plugin_metadataedit.cpp 
                                      subjects.cpp
                                      metadatacheckbox.cpp
                                      altlangstringedit.cpp
                                      multistringsedit.cpp
                                      multivaluesedit.cpp
                                      objectattributesedit.cpp
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
                                      iptcproperties.cpp
                                      iptcstatus.cpp
                                      iptcorigin.cpp
                                      iptcenvelope.cpp
                                      iptceditdialog.cpp 
                                      xmpeditdialog.cpp 
                                      xmpcontent.cpp
                                      xmpkeywords.cpp
                                      xmpcategories.cpp
                                      xmpsubjects.cpp
                                      xmporigin.cpp
                                      xmpcredits.cpp
                                      xmpstatus.cpp
                                      xmpproperties.cpp
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
                      kdcraw
                     )

INSTALL(TARGETS kipiplugin_metadataedit DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_metadataedit.desktop DESTINATION ${SERVICES_INSTALL_DIR})

INSTALL(FILES topicset.iptc-subjectcode.xml DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_metadataedit/data)
