INCLUDES    = -I$(top_srcdir)/libkipi $(all_includes)
METASOURCES = AUTO
SUBDIRS = data

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_batchprocessimages.la

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
					      batchprocessimagesdialog.cpp renameimagesoptionsdialog.cpp \
					      batchprocessimageslist.cpp

# Libs needed by the plugin
kipiplugin_batchprocessimages_la_LIBADD = $(LIB_KPARTS) -lkipi

# LD flags for the plugin
kipiplugin_batchprocessimages_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_batchprocessimages.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_batchprocessimages.pot

kipibatchprocessimagesicondir = $(kde_datadir)/kipi/icons
kipibatchprocessimagesicon_ICON = AUTO


