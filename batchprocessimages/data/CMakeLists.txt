
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### install files ###############

install(FILES  handcursor.png DESTINATION /share/apps/kipiplugin_batchprocessimages/data)



#original Makefile.am contents follow:

#kipibatchprocessimagespicdir = $(kde_datadir)/kipiplugin_batchprocessimages/data
#kipibatchprocessimagespic_DATA = handcursor.png
