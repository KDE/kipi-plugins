set(kipiplugin_timeadjust_PART_SRCS plugin_timeadjust.cpp timeadjustdialog.cpp)

kde4_add_plugin(kipiplugin_timeadjust ${kipiplugin_timeadjust_PART_SRCS})

target_link_libraries(kipiplugin_timeadjust ${KDE4_KPARTS_LIBS} ${QT_AND_KDECORE_LIBS} ${KIPI_LIBRARIES} ${KEXIV2_LIBRARIES} ${QT_QT3SUPPORT_LIBRARY})

install(TARGETS kipiplugin_timeadjust DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kipiplugin_timeadjust.desktop DESTINATION ${SERVICES_INSTALL_DIR})
