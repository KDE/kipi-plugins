#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${KSANE_INCLUDE_DIR})

# =======================================================
# Common source code

set(acquireimages_common_SRCS
    scandialog.cpp
    saveimgthread.cpp
   )

# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #270593)
foreach(_currentfile ${acquireimages_common_SRCS})
   if(NOT MSVC)
       set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
   endif()
endforeach()

KDE4_ADD_LIBRARY(libacquireimages STATIC ${acquireimages_common_SRCS})

target_link_libraries(libacquireimages
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KSANE_LIBRARY}
                      ${KEXIV2_LIBRARIES}
                      kipiplugins
                     )

# =======================================================
# Plugin

set(kipiplugin_acquireimages_PART_SRCS plugin_acquireimages.cpp)

KDE4_ADD_PLUGIN(kipiplugin_acquireimages ${kipiplugin_acquireimages_PART_SRCS})

target_link_libraries(kipiplugin_acquireimages libacquireimages)

configure_file(kipiplugin_acquireimages.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_acquireimages.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_acquireimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_acquireimages                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_acquireimagesui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)

# =======================================================
# Stand alone version

set(scangui_SRCS scangui.cpp)

KDE4_ADD_APP_ICON(scangui_SRCS "${KDE4_INSTALL_DIR}/share/icons/oxygen/*/devices/scanner.png")
KDE4_ADD_EXECUTABLE(scangui ${scangui_SRCS})

target_link_libraries(scangui libacquireimages)

install(TARGETS  scangui         ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS scangui.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
