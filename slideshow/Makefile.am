METASOURCES = AUTO

INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(IMLIB2_CFLAGS)  $(LIBKIPI_CFLAGS) \
	   $(LIBKDCRAW_CFLAGS) $(all_includes)

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_slideshow.la

kipiplugin_slideshow_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKDCRAW_LIBS_DEP) 

kipiplugin_slideshow_la_SOURCES = plugin_slideshow.cpp slideshow.cpp \
	slideshowconfig.cpp imlibiface.cpp slideshowgl.cpp toolbar.cpp slideshowconfigbase.ui

#kipiplugin_slideshow_la_LIBADD = -lkdefx $(LIB_IMLIB2) $(GL_LIBS) $(LIBKIPI_LIBS) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)
kipiplugin_slideshow_la_LIBADD = -lkdefx -lImlib2 $(GL_LIBS) $(LIBKIPI_LIBS) $(LIBKDCRAW_LIBS)  \
	                         $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

kipiplugin_slideshow_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins

kde_services_DATA = kipiplugin_slideshow.desktop

kipiplugin_slideshowicondir = $(kde_datadir)/kipiplugin_slideshow/icons
kipiplugin_slideshowicon_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_slideshow.pot

