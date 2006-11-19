INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(all_includes)

METASOURCES = AUTO
SUBDIRS = data

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_batchprocessimages.la
kipiplugin_batchprocessimages_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
# Srcs for the plugin
kipiplugin_batchprocessimages_la_SOURCES = plugin_batchprocessimages.cpp batchprocessimagesitem.cpp \
			   	           outputdialog.cpp imagepreview.cpp renameimagesdialog.cpp \
					   borderimagesdialog.cpp borderoptionsdialog.cpp \
					   convertimagesdialog.cpp convertoptionsdialog.cpp \
					   filterimagesdialog.cpp filteroptionsdialog.cpp \
					   colorimagesdialog.cpp coloroptionsdialog.cpp \
					   effectimagesdialog.cpp effectoptionsdialog.cpp \
					   recompressimagesdialog.cpp recompressoptionsdialog.cpp \
					   resizeimagesdialog.cpp resizeoptionsdialog.cpp \
					   batchprocessimagesdialog.cpp \
					   batchprocessimageslist.cpp \
					   renameimagesbase.ui renameimageswidget.cpp

# Libs needed by the plugin
kipiplugin_batchprocessimages_la_LIBADD = $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

# LD flags for the plugin
kipiplugin_batchprocessimages_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_batchprocessimages.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_batchprocessimages.pot

kipibatchprocessimagesicondir = $(kde_datadir)/kipiplugin_batchprocessimages/icons
kipibatchprocessimagesicon_ICON = AUTO


