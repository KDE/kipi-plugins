INCLUDES    = -I$(top_srcdir)/libkipi $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kipiplugin_gammacalibration.la
kipiplugin_gammacalibration_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
kipiplugin_gammacalibration_la_SOURCES = plugin_gammacalibration.cpp

kipiplugin_gammacalibration_la_LIBADD = $(LIB_KPARTS) $(LIBKIPI_LIBS)

kipiplugin_gammacalibration_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/kipi/plugins

kde_services_DATA = kipiplugin_gammacalibration.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_gammacalibration.pot

