INCLUDES    = -I$(top_srcdir)/libkipi $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_rawconverter.la

# Srcs for the plugin
kipiplugin_rawconverter_la_SOURCES = plugin_rawconverter.cpp \
	batchdialog.cpp singledialog.cpp previewwidget.cpp \
	processcontroller.cpp dmessagebox.cpp 

# Libs needed by the plugin
kipiplugin_rawconverter_la_LIBADD = $(LIB_KPARTS) -lkipi

# LD flags for the plugin
kipiplugin_rawconverter_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# binary
bin_PROGRAMS = kipidcrawclient

kipidcrawclient_SOURCES = dcrawprocess.cpp
kipidcrawclient_LDFLAGS = -lm -ljpeg -ltiff

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_rawconverter.desktop

kipirawconvertericondir = $(kde_datadir)/kipi/icons
kipirawconvertericon_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_rawconverter.pot

install-data-local:
	$(mkinstalldirs) $(mandir)/man1/
	$(INSTALL_DATA) $(srcdir)/kipidcrawclient.man.gz $(mandir)/man1/kipidcrawclient.1.gz

uninstall-local:
	-rm -f $(DESTDIR)/$(mandir)/man1/kipidcrawclient.1.gz
