INCLUDES    = $(all_includes)
METASOURCES = AUTO
SUBDIRS = data

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = digikamplugin_batchprocessimages.la

# Srcs for the plugin
digikamplugin_batchprocessimages_la_SOURCES = plugin_batchprocessimages.cpp batchprocessimagesitem.cpp \
					      outputdialog.cpp imagepreview.cpp renameimagesdialog.cpp \
					      borderimagesdialog.cpp borderoptionsdialog.cpp \
					      convertimagesdialog.cpp convertoptionsdialog.cpp \
					      filterimagesdialog.cpp filteroptionsdialog.cpp \
					      colorimagesdialog.cpp coloroptionsdialog.cpp \
					      effectimagesdialog.cpp effectoptionsdialog.cpp \
					      recompressimagesdialog.cpp recompressoptionsdialog.cpp \
					      resizeimagesdialog.cpp resizeoptionsdialog.cpp \
					      batchprocessimagesdialog.cpp renameimagesoptionsdialog.cpp \
					      batchprocessimageslist.cpp

# Libs needed by the plugin
digikamplugin_batchprocessimages_la_LIBADD = $(LIB_KPARTS) -ldigikam

# LD flags for the plugin
digikamplugin_batchprocessimages_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the .rc file in the Part's directory
pluginsdir = $(kde_datadir)/digikam/plugins
plugins_DATA = digikamplugin_batchprocessimages.rc

# Install the desktop file needed to detect the plugin
kde_services_DATA = digikamplugin_batchprocessimages.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/digikamplugin_batchprocessimages.pot

digikambatchprocessimagesicondir = $(kde_datadir)/digikam/icons
digikambatchprocessimagesicon_ICON = AUTO


