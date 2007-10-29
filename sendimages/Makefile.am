INCLUDES = $(LIBKEXIV2_CFLAGS) $(LIBKDCRAW_CFLAGS) $(KIPI_PLUGINS_COMMON_INCLUDE) \
	   $(LIBKIPI_CFLAGS) $(all_includes)

METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_sendimages.la
kipiplugin_sendimages_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP) $(LIBKDCRAW_LIBS_DEP) 

# Srcs for the plugin
kipiplugin_sendimages_la_SOURCES = plugin_sendimages.cpp sendimagesdialog.cpp \
                                   listimageserrordialog.cpp sendimages.cpp 

# Libs needed by the plugin
kipiplugin_sendimages_la_LIBADD = $(LIBKEXIV2_LIBS) $(LIBKIPI_LIBS) $(LIB_KIO) $(LIBKDCRAW_LIBS) \
	                          $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

# LD flags for the plugin
kipiplugin_sendimages_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) \
	                           -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_sendimages.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_sendimages.pot


