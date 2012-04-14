SUBDIRS(icons)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/importWizard)

SET(kipiplugin_flashexport_PART_SRCS plugin_flashexport.cpp
                                     simpleviewer.cpp
                                     flashmanager.cpp
                                     importWizard/intropage.cpp
                                     importWizard/generalpage.cpp
                                     importWizard/lookpage.cpp
                                     importWizard/selectionpage.cpp
                                     importWizard/firstrunpage.cpp
                                     importWizard/importwizarddlg.cpp
                                     importWizard/progresspage.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_flashexport ${kipiplugin_flashexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_flashexport 
                      ${KDE4_KPARTS_LIBS} 
                      ${KDE4_KIO_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      ${KDCRAW_LIBRARIES} 
                      kipiplugins
                     )

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

INSTALL(TARGETS kipiplugin_flashexport DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_flashexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})

INSTALL(FILES data/space.png       DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_flashexport/simpleviewer_html)
INSTALL(FILES data/index.template  DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_flashexport)
INSTALL(FILES data/index2.template DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_flashexport)
INSTALL(FILES data/index3.template DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_flashexport)
INSTALL(FILES data/index4.template DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_flashexport)
