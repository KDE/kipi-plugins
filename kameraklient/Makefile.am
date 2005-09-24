## Makefile.am for kameraklient
INCLUDES = $(GPHOTO_CFLAGS) $(LIBKIPI_CFLAGS) $(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES = kipiplugin_kameraklient.la
kipiplugin_kameraklient_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)

kipiplugin_kameraklient_la_LIBADD = $(LIB_KPARTS) $(LIB_KDEUI) $(LIBKIPI_LIBS) $(LIB_GPHOTO)

kipiplugin_kameraklient_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kde_services_DATA = kipiplugin_kameraklient.desktop

kipiplugin_kameraklient_la_SOURCES = kameraklient.cpp camerafolderitem.cpp camerafolderview.cpp \
				     cameraiconitem.cpp cameraiconview.cpp cameralist.cpp \
				     cameraselection.cpp cameratype.cpp cameraui.cpp \
				     dmessagebox.cpp gpcamera.cpp gpcontroller.cpp \
				     gpeventfilter.cpp gpfileitemcontainer.cpp gpfileiteminfo.cpp \
				     gpfileiteminfodlg.cpp gpiface.cpp gpmessages.cpp \
				     gpstatus.cpp savefiledialog.cpp setupcamera.cpp \
				     thumbitem.cpp thumbview.cpp

kipiplugin_kameraklientdir = $(kde_datadir)/kipiplugin_kameraklient

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_kameraklient.pot