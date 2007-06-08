include_directories(${KIPI_INCLUDE_DIR} ${KDE4_INCLUDES} ${QT4_INCLUDES})

set(kipiplugin_wallpaper_PART_SRCS plugin_wallpaper.cpp)

kde4_automoc(${kipiplugin_wallpaper_PART_SRCS})

kde4_add_plugin(kipiplugin_wallpaper ${kipiplugin_wallpaper_PART_SRCS})

target_link_libraries(kipiplugin_wallpaper ${QT_AND_KDECORE_LIBS} ${KIPI_LIBRARIES})

install(TARGETS kipiplugin_wallpaper DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kipiplugin_wallpaper.desktop DESTINATION ${SERVICES_INSTALL_DIR})



#original Makefile.am contents follow:

#INCLUDES    = $(LIBKIPI_CFLAGS) $(all_includes)
#METASOURCES = AUTO
#
#kde_module_LTLIBRARIES = kipiplugin_wallpaper.la
#kipiplugin_wallpaper_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
#kipiplugin_wallpaper_la_SOURCES = plugin_wallpaper.cpp
#
#kipiplugin_wallpaper_la_LIBADD = $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)
#
#kipiplugin_wallpaper_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#
#pluginsdir = $(kde_datadir)/kipi/plugins
#
#kde_services_DATA = kipiplugin_wallpaper.desktop
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_wallpaper.pot
#
