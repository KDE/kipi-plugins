INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(all_includes)

METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_sync.la

kipiplugin_sync_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) \
	$(top_builddir)/kipi-plugins/common/exiv2iface/libexiv2iface.la 

kipiplugin_sync_la_SOURCES = plugin_sync.cpp \
		sink.cpp sinkfactory.cpp sinks.cpp \
		sinklist.cpp sinklistbase.ui

kipiplugin_sync_la_LIBADD = $(top_builddir)/kipi-plugins/common/exiv2iface/libexiv2iface.la \
	-lkwalletclient $(LIB_KHTML) $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

kipiplugin_sync_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins

kde_services_DATA = kipiplugin_sync.desktop

#kipiplugin_syncpicsdir = $(kde_datadir)/kipiplugin_sync/pics
#kipiplugin_syncpics_DATA = gallery.png

#kipiplugin_syncicondir = $(kde_datadir)/kipiplugin_sync/icons
#kipiplugin_syncicon_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_sync.pot

