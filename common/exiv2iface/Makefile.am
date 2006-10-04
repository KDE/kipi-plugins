METASOURCES = AUTO

KDE_CXXFLAGS = $(USE_EXCEPTIONS) 

noinst_LTLIBRARIES = libexiv2iface.la

libexiv2iface_la_SOURCES = exiv2iface.cpp 

libexiv2iface_la_LDFLAGS = $(all_libraries) $(KDE_RPATH)

libexiv2iface_la_LIBADD = $(LIB_EXIV2) $(LIB_KDECORE) $(LIB_QT)

INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(all_includes)

