INCLUDES    = -I$(top_srcdir)/libkipi $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kipiplugin_wallpaper.la
kipiplugin_wallpaper_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
kipiplugin_wallpaper_la_SOURCES = plugin_wallpaper.cpp

kipiplugin_wallpaper_la_LIBADD = $(LIB_KPARTS) $(LIBKIPI_LIBS)

kipiplugin_wallpaper_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/kipi/plugins

kde_services_DATA = kipiplugin_wallpaper.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_wallpaper.pot

