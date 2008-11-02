set(kipiplugin_printwizard_SRCS
	cropframe.cpp
	wizard.cpp
	plugin_printwizard.cpp
	tphoto.cpp
	utils.cpp
	)

kde4_add_ui_files(kipiplugin_printwizard_SRCS
	intropage.ui
	infopage.ui
	photopage.ui
	croppage.ui
	)

kde4_add_plugin(kipiplugin_printwizard ${kipiplugin_printwizard_SRCS})

target_link_libraries(kipiplugin_printwizard
	${KDE4_KPARTS_LIBS}
        ${QT_AND_KDECORE_LIBS}
	${KIPI_LIBRARIES}
	${KEXIV2_LIBRARIES}
	kipiplugins
	)

install(TARGETS kipiplugin_printwizard
	DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kipiplugin_printwizard.desktop
	DESTINATION ${SERVICES_INSTALL_DIR})
