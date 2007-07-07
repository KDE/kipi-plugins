INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKEXIV2_CFLAGS) $(LIBKIPI_CFLAGS) $(all_includes)

METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_printwizard.la
kipiplugin_printwizard_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP)
kipiplugin_printwizard_la_SOURCES = frmprintwizardbase.ui plugin_printwizard.cpp frmprintwizard.cpp cropframe.cpp  tphoto.cpp utils.cpp

#LIBKIPI_LIBS = $(top_builddir)/libkipi/libkipi/libkipi.la
kipiplugin_printwizard_la_LIBADD = $(LIBKEXIV2_LIBS) -lkipiplugins \
				   $(LIB_KDEPRINT) $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

kipiplugin_printwizard_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) 

kde_services_DATA = kipiplugin_printwizard.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_printwizard.pot

