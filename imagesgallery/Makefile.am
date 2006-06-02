INCLUDES    = $(LIBKIPI_CFLAGS) $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_imagesgallery.la
kipiplugin_imagesgallery_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
# Srcs for the plugin
kipiplugin_imagesgallery_la_SOURCES = plugin_imagesgallery.cpp imgallerydialog.cpp \
                                      imagesgallery.cpp exifrestorer.cpp 

# Libs needed by the plugin
kipiplugin_imagesgallery_la_LIBADD = $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

# LD flags for the plugin
kipiplugin_imagesgallery_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_imagesgallery.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_imagesgallery.pot

