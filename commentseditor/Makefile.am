INCLUDES    = $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_commentseditor.la

kipiplugin_commentseditor_la_SOURCES = plugin_commentseditor.cpp \
				commentseditor.cpp

kipiplugin_commentseditor_la_LIBADD = $(LIB_KPARTS) -lkipi \
					$(LIB_IMLIB)  $(top_builddir)/libkipi/libkipi/libkipi.la

kipiplugin_commentseditor_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kde_services_DATA = kipiplugin_commentseditor.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_commentseditor.pot

