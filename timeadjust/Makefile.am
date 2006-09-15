INCLUDES = -I$(top_srcdir)/kipi-plugins/common/include \
           $(LIBKIPI_CFLAGS) $(LIBKEXIF_CFLAGS) $(all_includes)

METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_timeadjust.la
kipiplugin_timeadjust_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIF_LIBS_DEP)
# Srcs for the plugin
kipiplugin_timeadjust_la_SOURCES = plugin_timeadjust.cpp timeadjustdialog.cpp

# Libs needed by the plugin
kipiplugin_timeadjust_la_LIBADD = $(LIBKEXIF_LIBS) $(LIBKIPI_LIBS) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

# LD flags for the plugin
kipiplugin_timeadjust_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_timeadjust.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_timeadjust.pot

