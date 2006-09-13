INCLUDES    = -I$(top_srcdir)/kipi-plugins/common/include \
	          $(LIBKIPI_CFLAGS) $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_rawconverter.la
kipiplugin_rawconverter_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
# Srcs for the plugin
kipiplugin_rawconverter_la_SOURCES = plugin_rawconverter.cpp dcrawutils.cpp savesettingswidget.cpp \
	                                 batchdialog.cpp actionthread.cpp dcrawsettingswidget.cpp \
								     singledialog.cpp previewwidget.cpp 

# Libs needed by the plugin
kipiplugin_rawconverter_la_LIBADD = -ljpeg -lpng $(LIB_TIFF) $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

# LD flags for the plugin
kipiplugin_rawconverter_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_rawconverter.desktop

kipirawconvertericondir = $(kde_datadir)/kipiplugin_rawconverter/icons
kipirawconvertericon_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_rawconverter.pot

