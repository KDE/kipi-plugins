INCLUDES    = $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = digikamplugin_sendimages.la

# Srcs for the plugin
digikamplugin_sendimages_la_SOURCES = plugin_sendimages.cpp sendimagesdialog.cpp listimageserrordialog.cpp

# Libs needed by the plugin
digikamplugin_sendimages_la_LIBADD = $(LIB_KPARTS) -ldigikam

# LD flags for the plugin
digikamplugin_sendimages_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the .rc file in the Part's directory
pluginsdir = $(kde_datadir)/digikam/plugins
plugins_DATA = digikamplugin_sendimages.rc

# Install the desktop file needed to detect the plugin
kde_services_DATA = digikamplugin_sendimages.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/digikamplugin_sendimages.pot


