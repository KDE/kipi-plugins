# set the include path for X, qt and KDE
INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(LIBKDCRAW_CFLAGS) $(all_includes)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_viewer.la

kipiplugin_viewer_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKDCRAW_LIBS_DEP)
kipiplugin_viewer_la_SOURCES = ogl.cpp texture.cpp timer.cpp plugin_viewer.cpp help.ui
kipiplugin_viewer_la_LIBADD = $(LIBKIPI_LIBS) $(LIBKDCRAW_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) -lGL
kipiplugin_viewer_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries)

kde_services_DATA = kipiplugin_viewer.desktop


kipiviewericondir = $(kde_datadir)/kipiplugin_viewer/icons
kipiviewericon_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_viewer.pot


#icon is still missing