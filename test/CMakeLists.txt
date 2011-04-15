# * ============================================================
# *
# * Copyright (C) 2009,2010 by Michael G. Hansen <mike at mghansen dot de>
# *
# * This program is free software; you can redistribute it
# * and/or modify it under the terms of the GNU General
# * Public License as published by the Free Software Foundation;
# * either version 2, or (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * ============================================================

project( kipitest )

find_package( KDE4 REQUIRED )

include_directories( ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} )

FIND_PACKAGE(Kexiv2)
FIND_PACKAGE(Kipi)

ADD_DEFINITIONS(${KEXIV2_DEFINITIONS})

set(kipicmd_sources
    kipiinterface.cpp
    kipiimagecollectionshared.cpp
    kipiimageinfoshared.cpp
    kipiimagecollectionselector.cpp
    kipiuploadwidget.cpp
    kipitest-debug.cpp
    main.cpp
   )

kde4_add_executable( kipicmd ${kipicmd_sources} )
target_link_libraries( kipicmd ${KDE4_UI_LIBS} ${KDE4_KPARTS_LIBS} ${KEXIV2_LIBRARIES} ${KIPI_LIBRARIES} )

