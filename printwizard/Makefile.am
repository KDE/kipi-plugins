INCLUDES    =  $(all_includes) $(LIBKIPI_CFLAGS)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_printwizard.la
kipiplugin_printwizard_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
kipiplugin_printwizard_la_SOURCES = frmprintwizardbase.ui plugin_printwizard.cpp frmprintwizard.cpp cropframe.cpp  tphoto.cpp utils.cpp

#kipiplugin_printwizard_la_LIBADD = $(LIB_KPARTS) $(LIB_KDEPRINT) $(LIB_KDEUI) $(LIB_KDECORE) $(top_builddir)/libkipi/libkipi/libkipi.la
kipiplugin_printwizard_la_LIBADD = $(LIB_KPARTS) $(LIB_KDEPRINT) $(LIB_KDEUI) $(LIB_KDECORE) $(LIBKIPI_LIBS)

kipiplugin_printwizard_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kde_services_DATA = kipiplugin_printwizard.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_printwizard.pot

