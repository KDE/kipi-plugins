INCLUDES    = -I$(top_srcdir)/libkipi $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kipiplugin_miscsoperations.la
kipiplugin_miscsoperations_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
kipiplugin_miscsoperations_la_SOURCES = plugin_miscsoperations.cpp

kipiplugin_miscsoperations_la_LIBADD = $(LIB_KPARTS) $(LIBKIPI_LIBS)

kipiplugin_miscsoperations_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/kipi/plugins

kde_services_DATA = kipiplugin_miscsoperations.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_miscsoperations.pot

