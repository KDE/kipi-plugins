INCLUDES    = -I$(top_srcdir)/libkipi $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_jpeglossless.la

kipiplugin_jpeglossless_la_SOURCES = plugin_jpeglossless.cpp \
		actionthread.cpp utils.cpp transupp.c progressdlg.cpp \
		imagerotate.cpp  convert2grayscale.cpp imageflip.cpp \
		messagebox.cpp

kipiplugin_jpeglossless_la_LIBADD = $(LIB_KPARTS) \
					-ljpeg -ltiff  $(top_builddir)/libkipi/libkipi/libkipi.la

kipiplugin_jpeglossless_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kde_services_DATA = kipiplugin_jpeglossless.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_jpeglossless.pot

kipijpeglosslessicondir = $(kde_datadir)/kipi/icons
kipijpeglosslessicon_ICON = AUTO
