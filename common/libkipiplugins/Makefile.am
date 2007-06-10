METASOURCES = AUTO

KDE_CXXFLAGS = $(USE_EXCEPTIONS) 

lib_LTLIBRARIES = libkipiplugins.la

libkipiplugins_la_SOURCES = kpaboutdata.cpp 

libkipiplugins_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -version-info 0:1:0 -no-undefined

libkipiplugins_la_LIBADD = $(LIB_KDECORE) $(LIB_QT)

INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(all_includes) 

