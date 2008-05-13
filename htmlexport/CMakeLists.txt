subdirs(themes)

set(kipiplugin_htmlexport_SRCS
	abstractthemeparameter.cpp
	colorthemeparameter.cpp
	galleryinfo.cpp
	generator.cpp
	intthemeparameter.cpp
	listthemeparameter.cpp
	plugin.cpp
	stringthemeparameter.cpp
	theme.cpp
	wizard.cpp
	)

kde4_add_ui_files(kipiplugin_htmlexport_SRCS
	imagesettingspage.ui
	outputpage.ui
	themepage.ui
	themeparameterspage.ui
	)

kde4_add_kcfg_files(kipiplugin_htmlexport_SRCS
	htmlexportconfig.kcfgc
	)

kde4_add_plugin(kipiplugin_htmlexport ${kipiplugin_htmlexport_SRCS})

target_link_libraries(kipiplugin_htmlexport
	${KIPI_LIBRARIES}
	${LIBXSLT_LIBRARIES}
	exslt
	${LIBXML2_LIBRARIES}
	kipiplugins
	)

include_directories(
	${LIBXSLT_INCLUDE_DIR}
	${LIBXML2_INCLUDE_DIR}
	)

add_definitions(
	${LIBXSLT_DEFINITIONS}
	${LIBXML2_DEFINITIONS}
	)

install(TARGETS kipiplugin_htmlexport
	DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kipiplugin_htmlexport.desktop
	DESTINATION ${SERVICES_INSTALL_DIR})
