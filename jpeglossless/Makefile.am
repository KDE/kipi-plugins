METASOURCES  = AUTO
KDE_CXXFLAGS = $(USE_EXCEPTIONS)
INCLUDES     = $(LIBKIPI_CFLAGS) $(LIBKEXIF_CFLAGS) $(all_includes)

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_jpeglossless.la

kipiplugin_jpeglossless_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIF_LIBS_DEP)

kipiplugin_jpeglossless_la_SOURCES = plugin_jpeglossless.cpp jpegtransform.cpp \
		                     actionthread.cpp utils.cpp transupp.c \
		                     imagerotate.cpp convert2grayscale.cpp imageflip.cpp 

kipiplugin_jpeglossless_la_LIBADD = -ljpeg -lMagick++ $(LIBKEXIF_LIBS) $(LIBKIPI_LIBS) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)


kipiplugin_jpeglossless_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kde_services_DATA = kipiplugin_jpeglossless.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_jpeglossless.pot

kipijpeglosslessicondir = $(kde_datadir)/kipiplugin_jpeglossless/icons
kipijpeglosslessicon_ICON = AUTO
