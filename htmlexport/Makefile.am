SUBDIRS = themes
INCLUDES    = $(LIBKIPI_CFLAGS) $(all_includes) $(LIBXSLT_CFLAGS)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_htmlexport.la

kipiplugin_htmlexport_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
kipiplugin_htmlexport_la_SOURCES = \
	generator.cpp \
	themepage.ui \
	imagesettingspage.ui \
	outputpage.ui \
	plugin.cpp \
	wizard.cpp \
	theme.cpp \
	htmlexportconfig.kcfgc

kde_kcfg_DATA = \
	htmlexportconfig.kcfg

kipiplugin_htmlexport_la_LIBADD = -lexslt -lxml2 $(LIBXSLT_LIBS) $(LIBKIPI_LIBS) $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)
kipiplugin_htmlexport_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)


# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_htmlexport.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_htmlexport.pot
