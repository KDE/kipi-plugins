INCLUDES    = $(LIBKIPI_CFLAGS) $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_flickrexport.la

kipiplugin_flickrexport_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)

kipiplugin_flickrexport_la_SOURCES = plugin_flickrexport.cpp \
		flickrwindow.cpp login.cpp flickrtalker.cpp \
		flickrwidget.cpp mpform.cpp flickralbumdialog.ui \
		flickrviewitem.cpp	exifrestorer.cpp 

kipiplugin_flickrexport_la_LIBADD = $(LIB_KPARTS) $(LIBKIPI_LIBS) \
				$(LIB_KHTML)

kipiplugin_flickrexport_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kde_services_DATA = kipiplugin_flickrexport.desktop

#kipiplugin_flickrexporticondir = $(kde_datadir)/kipiplugin_flickrexport/icons
#kipiplugin_flickrexporticon_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_flickrexport.pot

