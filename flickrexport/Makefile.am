METASOURCES = AUTO

INCLUDES = $(LIBKEXIV2_CFLAGS) $(LIBKDCRAW_CFLAGS) $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(all_includes)

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_flickrexport.la

kipiplugin_flickrexport_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP) $(LIBKDCRAW_LIBS_DEP)

kipiplugin_flickrexport_la_SOURCES = plugin_flickrexport.cpp \
		                             flickrwindow.cpp login.cpp flickrtalker.cpp \
		                             flickrwidget.cpp mpform.cpp flickralbumdialog.ui \
		                             flickrviewitem.cpp  

kipiplugin_flickrexport_la_LIBADD = $(LIBKEXIV2_LIBS) $(LIBKDCRAW_LIBS) $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

kipiplugin_flickrexport_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins

kde_services_DATA = kipiplugin_flickrexport.desktop


messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_flickrexport.pot
