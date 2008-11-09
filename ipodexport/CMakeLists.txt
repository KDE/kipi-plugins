INCLUDE_DIRECTORIES( ${GPOD_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR} )
add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)
########### next target ###############

set(kipiplugin_ipodexport_PART_SRCS
    plugin_ipodexport.cpp
    ipodexportdialog.cpp
    ipodheader.cpp
    ipodlistitem.cpp
    imagelist.cpp)

kde4_add_plugin(kipiplugin_ipodexport ${kipiplugin_ipodexport_PART_SRCS})

target_link_libraries(kipiplugin_ipodexport kipiplugins ${KDE4_KDE3SUPPORT_LIBS} ${KDE4_KDEUI_LIBS} kio  ${GPOD_LIBRARIES})

install(TARGETS kipiplugin_ipodexport DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES  kipiplugin_ipodexport.desktop DESTINATION /share/services)

