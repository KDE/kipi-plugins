INCLUDES    = -I$(top_srcdir)/libkipi $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_findimages.la

kipiplugin_findimages_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)

# Srcs for the plugin
kipiplugin_findimages_la_SOURCES = plugin_findimages.cpp displaycompare.cpp \
                                   finddupplicateimages.cpp finddupplicatedialog.cpp

# Libs needed by the plugin
kipiplugin_findimages_la_LIBADD = $(LIB_KPARTS) $(LIBKIPI_LIBS)

# LD flags for the plugin
kipiplugin_findimages_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_findimages.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_findimages.pot

kipifindimagesicondir = $(kde_datadir)/kipiplugin_findimages/icons
kipifindimagesicon_ICON = AUTO
