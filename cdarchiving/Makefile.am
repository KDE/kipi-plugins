INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(all_includes)

METASOURCES = AUTO
SUBDIRS = autorun

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_cdarchiving.la
kipiplugin_cdarchiving_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)

# Srcs for the plugin
kipiplugin_cdarchiving_la_SOURCES = plugin_cdarchiving.cpp cdarchivingdialog.cpp cdarchiving.cpp 

# Libs needed by the plugin
kipiplugin_cdarchiving_la_LIBADD = $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

# LD flags for the plugin
kipiplugin_cdarchiving_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_cdarchiving.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_cdarchiving.pot

kipiimagebrokenpicdir = $(kde_datadir)/kipi/data
kipiimagebrokenpic_DATA = image_broken.png

kipiHTML401picdir = $(kde_datadir)/kipi
kipiHTML401pic_DATA = valid-html401.png gohome.png up.png
