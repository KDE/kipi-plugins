INCLUDES    = -I$(top_srcdir)/libkipi $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kipiplugin_diroperations.la
kipiplugin_diroperations_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
kipiplugin_diroperations_la_SOURCES = plugin_diroperations.cpp

kipiplugin_diroperations_la_LIBADD = $(LIB_KPARTS) $(LIBKIPI_LIBS)

kipiplugin_diroperations_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/kipi/plugins

kde_services_DATA = kipiplugin_diroperations.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_diroperations.pot

