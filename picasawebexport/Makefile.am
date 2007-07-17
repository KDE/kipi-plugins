METASOURCES = AUTO

INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(all_includes)

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_picasawebexport.la

kipiplugin_picasawebexport_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)

kipiplugin_picasawebexport_la_SOURCES = plugin_picasawebexport.cpp PicasawebNewAlbumDialog.ui\ 
		                     picasawebwindow.cpp picasaweblogin.cpp picasawebtalker.cpp \
		                     picasawebwidget.cpp mpform.cpp picasawebviewitem.cpp exifrestorer.cpp uploadwidget.ui
#PicasawebNewAlbumDialog.ui

kipiplugin_picasawebexport_la_LIBADD = $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

kipiplugin_picasawebexport_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins

kde_services_DATA = kipiplugin_picasawebexport.desktop

#kipiplugin_picasawebexporticondir = $(kde_datadir)/kipiplugin_picasawebexport/icons
#kipiplugin_picasawebexporticon_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_picasawebexport.pot

