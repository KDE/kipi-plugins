INCLUDES    = -I$(top_srcdir)/libkipi $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = digikamplugin_rawconverter.la

digikamplugin_rawconverter_la_SOURCES = plugin_rawconverter.cpp \
	batchdialog.cpp singledialog.cpp previewwidget.cpp \
	processcontroller.cpp dmessagebox.cpp 

digikamplugin_rawconverter_la_LIBADD = $(LIB_KPARTS) -lkipi

digikamplugin_rawconverter_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# binary
bin_PROGRAMS = digikamdcrawclient

digikamdcrawclient_SOURCES = dcrawprocess.cpp
digikamdcrawclient_LDFLAGS = -lm -ljpeg -ltiff

# Install the .rc file in the Part's directory 
pluginsdir = $(kde_datadir)/digikam/plugins
plugins_DATA = digikamplugin_rawconverter.rc

kde_services_DATA = digikamplugin_rawconverter.desktop

digikamrawconvertericondir = $(kde_datadir)/digikam/icons
digikamrawconvertericon_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/digikamplugin_rawconverter.pot

install-data-local:
	$(mkinstalldirs) $(mandir)/man1/
	$(INSTALL_DATA) $(srcdir)/digikamdcrawclient.man.gz $(mandir)/man1/digikamdcrawclient.1.gz

uninstall-local:
	-rm -f $(DESTDIR)/$(mandir)/man1/digikamdcrawclient.1.gz
