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
bin_PROGRAMS = digikamdcrawclient

digikamdcrawclient_SOURCES = dcrawprocess.cpp
digikamdcrawclient_LDFLAGS = -lm -ljpeg -ltiff

# Install the .rc file in the Part's directory 
pluginsdir = $(kde_datadir)/digikam/plugins
plugins_DATA = digikamplugin_rawconverter.rc

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_rawconverter.desktop

kipirawconvertericondir = $(kde_datadir)/digikam/icons
kipirawconvertericon_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_rawconverter.pot

install-data-local:
	$(mkinstalldirs) $(mandir)/man1/
	$(INSTALL_DATA) $(srcdir)/digikamdcrawclient.man.gz $(mandir)/man1/digikamdcrawclient.1.gz

uninstall-local:
	-rm -f $(DESTDIR)/$(mandir)/man1/digikamdcrawclient.1.gz
