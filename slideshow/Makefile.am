INCLUDES    = $(IMLIB_CFLAGS) $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_slideshow.la

kipiplugin_slideshow_la_SOURCES = plugin_slideshow.cpp \
	slideshow.cpp slideshowconfig.cpp imlibiface.cpp \
	slideshowgl.cpp

kipiplugin_slideshow_la_LIBADD = $(LIB_KPARTS) -lkipi \
				$(LIB_IMLIB) $(GL_LIBS) -lkipi

kipiplugin_slideshow_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kde_services_DATA = kipiplugin_slideshow.desktop

kipiplugin_slideshowicondir = $(kde_datadir)/kipi/icons
kipiplugin_slideshowicon_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_slideshow.pot

