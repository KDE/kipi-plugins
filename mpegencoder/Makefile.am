INCLUDES    = $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_mpegencoder.la

# Srcs for the plugin
kipiplugin_mpegencoder_la_SOURCES = plugin_mpegencoder.cpp \
                                       kimg2mpg.cpp kshowdebuggingoutput.cpp \
                                       optionsdialog.cpp checkbinprog.cpp

# Libs needed by the plugin
kipiplugin_mpegencoder_la_LIBADD = $(LIB_KPARTS) -lkipi $(top_builddir)/libkipi/libkipi/libkipi.la

# LD flags for the plugin
kipiplugin_mpegencoder_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_mpegencoder.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_mpegencoder.pot

install-data-local:
	$(mkinstalldirs) $(mandir)/man1/
	$(INSTALL_DATA) $(srcdir)/images2mpg.man.gz $(mandir)/man1/images2mpg.1.gz

install-exec-local:
	$(mkinstalldirs) $(kde_bindir)/
	$(INSTALL_SCRIPT) $(srcdir)/images2mpg $(kde_bindir)/images2mpg

uninstall-local:
	-rm -f $(DESTDIR)$(kde_bindir)/images2mpg
	-rm -f $(DESTDIR)$(mandir)/man1/images2mpg.1
