METASOURCES = AUTO
SUBDIRS = profiles pics

INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(LIBKEXIV2_CFLAGS) \
           $(LIBKDCRAW_CFLAGS) $(all_includes)

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_rawconverter.la

kipiplugin_rawconverter_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP) $(LIBKDCRAW_LIBS_DEP) 

# Srcs for the plugin
kipiplugin_rawconverter_la_SOURCES = plugin_rawconverter.cpp rawdecodingiface.cpp savesettingswidget.cpp \
	                             actionthread.cpp previewwidget.cpp iccjpeg.c \
	                             batchdialog.cpp singledialog.cpp 

# Libs needed by the plugin
kipiplugin_rawconverter_la_LIBADD = -ljpeg -lpng $(LIBKEXIV2_LIBS) $(LIBKDCRAW_LIBS) $(LIBKIPI_LIBS) \
	                            $(LIB_TIFF) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

# LD flags for the plugin
kipiplugin_rawconverter_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_rawconverter.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_rawconverter.pot
