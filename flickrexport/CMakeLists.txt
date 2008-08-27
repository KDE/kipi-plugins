add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

set(kipiplugin_flickrexport_PART_SRCS
    plugin_flickrexport.cpp
    imageslist.cpp
    flickrwindow.cpp
    login.cpp
    flickrtalker.cpp
    flickrwidget.cpp
    mpform.cpp
    flickrviewitem.cpp)

kde4_add_ui3_files(kipiplugin_flickrexport_PART_SRCS flickralbumdialog.ui)

kde4_add_plugin(kipiplugin_flickrexport ${kipiplugin_flickrexport_PART_SRCS})

target_link_libraries(
    kipiplugin_flickrexport
    kipiplugins
    ${KIPI_LIBRARIES}
    ${KEXIV2_LIBRARIES}
    ${KDCRAW_LIBRARIES}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KDEUI_LIBS}
    )

install(TARGETS kipiplugin_flickrexport DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES  kipiplugin_flickrexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
