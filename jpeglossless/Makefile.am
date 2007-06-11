SUBDIRS = pics
METASOURCES = AUTO
INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKEXIV2_CFLAGS) $(LIBKDCRAW_CFLAGS) $(LIBKIPI_CFLAGS) $(all_includes)

# --enable-final triggers: http://bugs.kde.org/show_bug.cgi?id=126326
# digikam: camera download: auto-rotated images loose EXIF info ...
# So make sure nofinal is always used here!
KDE_OPTIONS = nofinal

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_jpeglossless.la

kipiplugin_jpeglossless_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP) $(LIBKDCRAW_LIBS_DEP)

kipiplugin_jpeglossless_la_SOURCES = plugin_jpeglossless.cpp jpegtransform.cpp \
	                             actionthread.cpp utils.cpp transupp.cpp \
	                             imagerotate.cpp convert2grayscale.cpp imageflip.cpp 

kipiplugin_jpeglossless_la_LIBADD = -ljpeg $(LIBKIPI_LIBS) $(LIBKEXIV2_LIBS) $(LIB_KDEUI) \
	                            $(LIBKDCRAW_LIBS) $(LIB_KDECORE) $(LIB_QT)


kipiplugin_jpeglossless_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kde_services_DATA = kipiplugin_jpeglossless.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_jpeglossless.pot

