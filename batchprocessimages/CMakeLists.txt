
find_package(KDE4 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

include(KDE4Defaults)

include(MacroLibrary)

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(data)


########### next target ###############

set(kipiplugin_batchprocessimages_PART_SRCS
    plugin_batchprocessimages.cpp
    batchprocessimagesitem.cpp
    outputdialog.cpp
    imagepreview.cpp
    renameimagesdialog.cpp
    borderimagesdialog.cpp
    borderoptionsdialog.cpp
    convertimagesdialog.cpp
    convertoptionsdialog.cpp
    filterimagesdialog.cpp
    filteroptionsdialog.cpp
    colorimagesdialog.cpp
    coloroptionsdialog.cpp
    effectimagesdialog.cpp
    effectoptionsdialog.cpp
    recompressimagesdialog.cpp
    recompressoptionsdialog.cpp
    resizeimagesdialog.cpp
    resizeoptionsdialog.cpp
    batchprocessimagesdialog.cpp
    batchprocessimageslist.cpp
    renameimageswidget.cpp)

kde4_add_ui_files(kipiplugin_batchprocessimages_PART_SRCS renameimagesbase.ui)

kde4_add_plugin(kipiplugin_batchprocessimages ${kipiplugin_batchprocessimages_PART_SRCS})

target_link_libraries(kipiplugin_batchprocessimages ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS} kipiplugins)

install(TARGETS kipiplugin_batchprocessimages DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES  kipiplugin_batchprocessimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES  DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_batchprocessimages/icons)



#original Makefile.am contents follow:

#INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(LIBKEXIV2_CFLAGS) $(all_includes)
#
#METASOURCES = AUTO
#SUBDIRS = data
#
## Install this plugin in the KDE modules directory
#kde_module_LTLIBRARIES = kipiplugin_batchprocessimages.la
#kipiplugin_batchprocessimages_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP)
## Srcs for the plugin
#kipiplugin_batchprocessimages_la_SOURCES = plugin_batchprocessimages.cpp batchprocessimagesitem.cpp \
#			   	           outputdialog.cpp imagepreview.cpp renameimagesdialog.cpp \
#					   borderimagesdialog.cpp borderoptionsdialog.cpp \
#					   convertimagesdialog.cpp convertoptionsdialog.cpp \
#					   filterimagesdialog.cpp filteroptionsdialog.cpp \
#					   colorimagesdialog.cpp coloroptionsdialog.cpp \
#					   effectimagesdialog.cpp effectoptionsdialog.cpp \
#					   recompressimagesdialog.cpp recompressoptionsdialog.cpp \
#					   resizeimagesdialog.cpp resizeoptionsdialog.cpp \
#					   batchprocessimagesdialog.cpp \
#					   batchprocessimageslist.cpp \
#					   renameimagesbase.ui renameimageswidget.cpp
#
## Libs needed by the plugin
#kipiplugin_batchprocessimages_la_LIBADD = $(LIBKIPI_LIBS) $(LIBKEXIV2_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)
#
## LD flags for the plugin
#kipiplugin_batchprocessimages_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins
#
## Install the desktop file needed to detect the plugin
#kde_services_DATA = kipiplugin_batchprocessimages.desktop
#
## i18n translation messages
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_batchprocessimages.pot
#
#kipibatchprocessimagesicondir = $(kde_datadir)/kipiplugin_batchprocessimages/icons
#kipibatchprocessimagesicon_ICON = AUTO
#
#
