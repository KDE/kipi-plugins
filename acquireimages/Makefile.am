INCLUDES    = $(all_includes) $(LIBKIPI_CFLAGS)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_acquireimages.la
kipiplugin_acquireimages_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
# Srcs for the plugin
kipiplugin_acquireimages_la_SOURCES = plugin_acquireimages.cpp acquireimagedialog.cpp screenshotdialog.cpp

# Libs needed by the plugin
kipiplugin_acquireimages_la_LIBADD = $(LIB_KPARTS) -ltiff  $(LIBKIPI_LIBS)

# LD flags for the plugin
kipiplugin_acquireimages_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_acquireimages.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_acquireimages.pot


