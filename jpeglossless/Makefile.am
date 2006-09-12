METASOURCES  = AUTO
INCLUDES     = -I$(top_srcdir)/kipi-plugins/common/include \
	           $(LIBKIPI_CFLAGS) $(LIBKEXIF_CFLAGS) $(all_includes)

# --enable-final triggers: http://bugs.kde.org/show_bug.cgi?id=126326
# digikam: camera download: auto-rotated images loose EXIF info ...
# So make sure nofinal is always used here!
KDE_OPTIONS = nofinal

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_jpeglossless.la

kipiplugin_jpeglossless_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIF_LIBS_DEP)

kipiplugin_jpeglossless_la_SOURCES = plugin_jpeglossless.cpp jpegtransform.cpp \
		                             actionthread.cpp utils.cpp transupp.cpp \
		                             imagerotate.cpp convert2grayscale.cpp imageflip.cpp 

kipiplugin_jpeglossless_la_LIBADD = -ljpeg $(LIBKEXIF_LIBS) $(LIBKIPI_LIBS) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)


kipiplugin_jpeglossless_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kde_services_DATA = kipiplugin_jpeglossless.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_jpeglossless.pot

kipijpeglosslessicondir = $(kde_datadir)/kipiplugin_jpeglossless/icons
kipijpeglosslessicon_ICON = AUTO
