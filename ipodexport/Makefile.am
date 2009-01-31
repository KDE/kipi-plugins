
INCLUDES    = $(KIPI_PLUGINS_COMMON_INCLUDE) \
              $(LIBKIPI_CFLAGS) \
              $(LIBGPOD_CFLAGS) \
              $(GDK_CFLAGS) \
              $(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES = kipiplugin_ipodexport.la
kipiplugin_ipodexport_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)

kipiplugin_ipodexport_la_SOURCES = plugin_ipodexport.cpp \
                                   ipodexportdialog.cpp \
                                   ipodheader.cpp \
                                   ipodlistitem.cpp \
                                   imagelist.cpp

kipiplugin_ipodexport_la_LIBADD  = $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

kipiplugin_ipodexport_la_LDFLAGS = -module $(KDE_PLUGIN) $(LIBGPOD_LIBS) $(GDK_LIBS) $(all_libraries)

kde_services_DATA = kipiplugin_ipodexport.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_ipodexport.pot

