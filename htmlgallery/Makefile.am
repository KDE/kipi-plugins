SUBDIRS = themes
INCLUDES    = $(LIBKIPI_CFLAGS) $(all_includes) $(LIBXSLT_CFLAGS)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_htmlgallery.la

kipiplugin_htmlgallery_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
kipiplugin_htmlgallery_la_SOURCES = \
	generator.cpp \
	imagesettingspage.ui \
	outputpage.ui \
	plugin.cpp \
	wizard.cpp
kipiplugin_htmlgallery_la_LIBADD = $(LIB_KPARTS) $(LIBKIPI_LIBS) 
kipiplugin_htmlgallery_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) -lexslt $(LIBXSLT_LIBS)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_htmlgallery.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_htmlgallery.pot
