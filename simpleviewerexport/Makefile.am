INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(all_includes)

METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_simpleviewer.la
kipiplugin_simpleviewer_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
# Srcs for the plugin
kipiplugin_simpleviewer_la_SOURCES = plugin_simpleviewer.cpp svedialog.cpp \
	simpleviewerexport.cpp firstrundlg.cpp

# Libs needed by the plugin
kipiplugin_simpleviewer_la_LIBADD = $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

# LD flags for the plugin
kipiplugin_simpleviewer_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_simpleviewer.desktop

# Install the SimpleViewer files
kipiplugin_simpleviewer_svdir = $(kde_datadir)/kipiplugin_simpleviewerexport/simpleviewer_html
kipiplugin_simpleviewer_sv_DATA = upgrade.html space.png

kipiplugin_simpleviewer_svtemplatedir = $(kde_datadir)/kipiplugin_simpleviewerexport
kipiplugin_simpleviewer_svtemplate_DATA = index.template

# i18n translation messages 
#Nota : no need a translation file for this plugin.
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_simpleviewer.pot

