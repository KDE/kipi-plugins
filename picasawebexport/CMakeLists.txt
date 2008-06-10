

########### next target ###############

set(kipiplugin_picasawebexport_PART_SRCS
    plugin_picasawebexport.cpp
    picasawebwindow.cpp
    picasaweblogin.cpp
    picasawebtalker.cpp
    picasawebwidget.cpp
    mpform.cpp
    picasawebviewitem.cpp)

kde4_add_ui3_files(kipiplugin_picasawebexport_PART_SRCS PicasawebNewAlbumDialog.ui uploadwidget.ui)

kde4_add_plugin(kipiplugin_picasawebexport ${kipiplugin_picasawebexport_PART_SRCS})

target_link_libraries(kipiplugin_picasawebexport ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} kipiplugins)

install(TARGETS kipiplugin_picasawebexport DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES  kipiplugin_picasawebexport.desktop DESTINATION ${SERVICES_INSTALL_DIR} )

