# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
# noinst_HEADERS = ogl.h texture.h timer.h texcoord.h plugin_viewer.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_viewer.la

kipiplugin_viewer_la_SOURCES = ogl.cpp texture.cpp timer.cpp plugin_viewer.cpp
kipiplugin_viewer_la_LIBADD = $(LIB KPARTS) $(LIBKIPI_LIBS) -lGL
kipiplugin_viewer_la_LDFLAGS = -module $(KDE PLUGIN) $(all libraries)

kde_services_DATA = kipiplugin_viewer.desktop



# messages: rc.cpp
# 	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
# 	$(XGETTEXT) *.cpp -o $(podir)/ogl.pot

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_viewer.pot


# KDE_ICON = AUTO
# 
# #########################################################################
# # APPLICATION SECTION
# #########################################################################
# # this is the program that gets installed.  it's name is used for all
# # of the other Makefile.am variables
# bin_PROGRAMS = ogl
# 
# # the application source, library search path, and link libraries
# ogl_SOURCES = main.cpp ogl.cpp texture.cpp timer.cpp texcoord.cpp \
# 	plugin_viewer.cpp kipi_viewer.desktop
# ogl_LDFLAGS = $(KDE_RPATH) $(all_libraries)
# ogl_LDADD = $(LIB_KDEUI)
# 
# # this is where the desktop file will go 
# shelldesktopdir   = $(kde_appsdir)/Utilities
# shelldesktop_DATA = ogl.desktop
# 
# # this is where the shell's XML-GUI resource file goes
# shellrcdir   = $(kde_datadir)/ogl
# shellrc_DATA = oglui.rc

