INCLUDES    = -I$(top_srcdir)/libkipi $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_helloworld.la

# Srcs for the plugin
kipiplugin_helloworld_la_SOURCES = plugin_helloworld.cpp 

# Libs needed by the plugin
kipiplugin_helloworld_la_LIBADD = $(LIB_KPARTS) -lkipi

# LD flags for the plugin
kipiplugin_helloworld_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_helloworld.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_helloworld.pot

