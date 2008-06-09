

########### next target ###############

set(kipiplugin_flickrexport_PART_SRCS
    plugin_flickrexport.cpp
    imageslist.cpp
    flickrwindow.cpp
    login.cpp
    flickrtalker.cpp
    flickrwidget.cpp
    mpform.cpp
    flickrviewitem.cpp)

kde4_add_ui3_files(kipiplugin_flickrexport_PART_SRCS flickralbumdialog.ui)

kde4_add_plugin(kipiplugin_flickrexport ${kipiplugin_flickrexport_PART_SRCS})

target_link_libraries(kipiplugin_flickrexport  ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS} kipiplugins)

install(TARGETS kipiplugin_flickrexport DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES  kipiplugin_flickrexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})



#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#INCLUDES = $(LIBKEXIV2_CFLAGS) $(LIBKDCRAW_CFLAGS) $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(all_includes)
#
## Install this plugin in the KDE modules directory
#kde_module_LTLIBRARIES = kipiplugin_flickrexport.la
#
#kipiplugin_flickrexport_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP) $(LIBKDCRAW_LIBS_DEP)
#
#kipiplugin_flickrexport_la_SOURCES = plugin_flickrexport.cpp imageslist.cpp \
#		                             flickrwindow.cpp login.cpp flickrtalker.cpp \
#		                             flickrwidget.cpp mpform.cpp flickralbumdialog.ui \
#		                             flickrviewitem.cpp  
#
#kipiplugin_flickrexport_la_LIBADD = $(LIBKEXIV2_LIBS) $(LIBKDCRAW_LIBS) $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)
#
#kipiplugin_flickrexport_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins
#
#kde_services_DATA = kipiplugin_flickrexport.desktop
#
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_flickrexport.pot
