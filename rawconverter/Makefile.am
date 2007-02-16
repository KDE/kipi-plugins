# Requires by dcraw.c implementation.
KDE_OPTIONS = nofinal

METASOURCES = AUTO
SUBDIRS = profiles

INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(LIBKEXIV2_CFLAGS) $(all_includes)

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_rawconverter.la

kipiplugin_rawconverter_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP) 

# Srcs for the plugin
kipiplugin_rawconverter_la_SOURCES = plugin_rawconverter.cpp dcrawiface.cpp savesettingswidget.cpp \
	                             batchdialog.cpp actionthread.cpp dcrawsettingswidget.cpp \
	                             singledialog.cpp previewwidget.cpp iccjpeg.c dcrawbinary.cpp 

# Libs needed by the plugin
kipiplugin_rawconverter_la_LIBADD = -ljpeg -lpng $(LIBKEXIV2_LIBS) \
	                            $(LIB_TIFF) $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

# LD flags for the plugin
kipiplugin_rawconverter_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_rawconverter.desktop

kipirawconvertericondir = $(kde_datadir)/kipiplugin_rawconverter/icons
kipirawconvertericon_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_rawconverter.pot

#-- dcraw binary -------------------------------------------------

# RawConverter plugin use a dedicaced dcraw binary program to run. This is mandatory 
# since dcraw author have broken the command line options 
# compatibility with the 8.x serie.
# Also, dcraw is not available like a library.
# The dcraw.c source code is just a copy of official implementation, but 
# this one have been completly tested with this plugin. 
# Note: dcraw project page is http://cybercom.net/~dcoffin

bin_PROGRAMS = kipidcraw

# This line is require to disable color management support (using LCMS library) witch 
# is not yet supported by plugin.
kipidcraw_CFLAGS = -DNO_LCMS -w 

kipidcraw_SOURCES = dcraw.c

kipidcraw_LDFLAGS = $(all_libraries) $(LIBJPEG) -lm


