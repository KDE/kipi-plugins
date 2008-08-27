

########### next target ###############
add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)


set(kipiplugin_picasawebexport_PART_SRCS
    plugin_picasawebexport.cpp
    picasawebwindow.cpp
    picasaweblogin.cpp
    picasawebtalker.cpp
    picasawebwidget.cpp
    mpform.cpp
    picasawebviewitem.cpp)

kde4_add_ui_files(kipiplugin_picasawebexport_PART_SRCS uploadwidget.ui)

kde4_add_ui3_files(kipiplugin_picasawebexport_PART_SRCS PicasawebNewAlbumDialog.ui )

kde4_add_plugin(kipiplugin_picasawebexport ${kipiplugin_picasawebexport_PART_SRCS})

target_link_libraries(
    kipiplugin_picasawebexport
    kipiplugins
    ${KDCRAW_LIBRARIES}
    ${KIPI_LIBRARIES}
    ${KEXIV2_LIBRARIES}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    )

install(TARGETS kipiplugin_picasawebexport DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES  kipiplugin_picasawebexport.desktop DESTINATION ${SERVICES_INSTALL_DIR} )

