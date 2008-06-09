
find_package(KDE4 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include(KDE4Defaults)

include(MacroLibrary)

include(ConvenienceLibs.cmake)

include(ManualStuff.cmake)

include(ConfigureChecks.cmake)

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(kipiplugin_picasawebexport_PART_SRCS
    plugin_picasawebexport.cpp
    picasawebwindow.cpp
    picasaweblogin.cpp
    picasawebtalker.cpp
    picasawebwidget.cpp
    mpform.cpp
    picasawebviewitem.cpp)

kde4_add_ui_files(kipiplugin_picasawebexport_PART_SRCS PicasawebNewAlbumDialog.ui uploadwidget.ui)

kde4_add_plugin(kipiplugin_picasawebexport ${kipiplugin_picasawebexport_PART_SRCS})

target_link_libraries(kipiplugin_picasawebexport ${KDE4_KDECORE_LIBS} kio kdeui kipiplugins)

install(TARGETS kipiplugin_picasawebexport DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES  kipiplugin_picasawebexport.desktop DESTINATION /share/services)



#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#INCLUDES = $(LIBKEXIV2_CFLAGS) $(LIBKDCRAW_CFLAGS) $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(all_includes)
#
## Install this plugin in the KDE modules directory
#kde_module_LTLIBRARIES = kipiplugin_picasawebexport.la
#
#kipiplugin_picasawebexport_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP) $(LIBKDCRAW_LIBS_DEP)
#
#kipiplugin_picasawebexport_la_SOURCES = plugin_picasawebexport.cpp \
#		                     picasawebwindow.cpp picasaweblogin.cpp picasawebtalker.cpp \
#		                     picasawebwidget.cpp mpform.cpp picasawebviewitem.cpp \
#		                     PicasawebNewAlbumDialog.ui uploadwidget.ui
#
#kipiplugin_picasawebexport_la_LIBADD = $(LIBKEXIV2_LIBS) $(LIBKDCRAW_LIBS) $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)
#
#kipiplugin_picasawebexport_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins
#
#kde_services_DATA = kipiplugin_picasawebexport.desktop
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_picasawebexport.pot
