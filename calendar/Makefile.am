INCLUDES    = $(all_includes) $(LIBKIPI_CFLAGS)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_calendar.la
kipiplugin_calendar_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
kipiplugin_calendar_la_SOURCES = plugin_calendar.cpp \
		calwizard.cpp caltemplate.cpp calsettings.cpp \
		calwidget.cpp calpainter.cpp calselect.cpp \
		monthwidget.cpp

kipiplugin_calendar_la_LIBADD = $(LIB_KPARTS) \
				-lkdeprint $(LIBKIPI_LIBS)

kipiplugin_calendar_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kde_services_DATA = kipiplugin_calendar.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_calendar.pot

