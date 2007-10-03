INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKEXIV2_CFLAGS) $(LIBKIPI_CFLAGS) $(all_includes)

METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_metadataedit.la

kipiplugin_metadataedit_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP) 

# Srcs for the plugin
kipiplugin_metadataedit_la_SOURCES = plugin_metadataedit.cpp metadatacheckbox.cpp \
	                             exifeditdialog.cpp exifcaption.cpp \
	                             exifdatetime.cpp exifadjust.cpp \
	                             exiflens.cpp exifdevice.cpp exiflight.cpp \
	                             iptceditdialog.cpp iptccaption.cpp iptcsubjects.cpp \
	                             iptccredits.cpp iptcstatus.cpp iptcorigin.cpp \
	                             iptcdatetime.cpp iptckeywords.cpp iptccategories.cpp \
				     commenteditdialog.cpp commentremovedialog.cpp 

# Libs needed by the plugin
kipiplugin_metadataedit_la_LIBADD = $(LIBKEXIV2_LIBS) \
	       -lkipiplugins $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

# LD flags for the plugin
kipiplugin_metadataedit_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_metadataedit.desktop

# Icons set for the plugin.
kipiplugin_metadataediticondir = $(kde_datadir)/kipiplugin_metadataedit/icons
kipiplugin_metadataediticon_ICON = AUTO


# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_metadataedit.pot

