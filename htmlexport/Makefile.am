SUBDIRS = themes
INCLUDES    = $(LIBKIPI_CFLAGS) $(all_includes) $(LIBXSLT_CFLAGS)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_htmlexport.la

kipiplugin_htmlexport_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
kipiplugin_htmlexport_la_SOURCES = \
	generator.cpp \
	imagesettingspage.ui \
	outputpage.ui \
	plugin.cpp \
	wizard.cpp \
	theme.cpp
kipiplugin_htmlexport_la_LIBADD = $(LIB_KPARTS) $(LIBKIPI_LIBS) 
kipiplugin_htmlexport_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) -lexslt $(LIBXSLT_LIBS)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_htmlexport.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_htmlexport.pot
