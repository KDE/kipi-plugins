INCLUDES    = -I$(top_srcdir) -I$(top_srcdir)/libkipi $(all_includes) \
	$(LIBKEXIF_CFLAGS)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_jpeglossless.la

kipiplugin_jpeglossless_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIF_LIBS_DEP)

kipiplugin_jpeglossless_la_SOURCES = plugin_jpeglossless.cpp \
		                     actionthread.cpp utils.cpp transupp.c progressdlg.cpp \
		                     imagerotate.cpp  convert2grayscale.cpp imageflip.cpp \
		                     messagebox.cpp jpegtransform.cpp

kipiplugin_jpeglossless_la_LIBADD = $(LIB_KPARTS) \
				    -ljpeg -ltiff $(LIBKIPI_LIBS)

kipiplugin_jpeglossless_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kde_services_DATA = kipiplugin_jpeglossless.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_jpeglossless.pot

kipijpeglosslessicondir = $(kde_datadir)/kipiplugin_jpeglossless/icons
kipijpeglosslessicon_ICON = AUTO
