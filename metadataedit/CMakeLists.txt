#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/exif
                    ${CMAKE_CURRENT_SOURCE_DIR}/iptc
                    ${CMAKE_CURRENT_SOURCE_DIR}/xmp
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                   )

set(kipiplugin_metadataedit_PART_SRCS plugin/plugin_metadataedit.cpp
                                      plugin/metadatacheckbox.cpp
                                      plugin/altlangstringedit.cpp
                                      plugin/multistringsedit.cpp
                                      plugin/multivaluesedit.cpp
                                      plugin/objectattributesedit.cpp
                                      plugin/metadataedit.cpp
                                      exif/exifeditwidget.cpp
                                      exif/exifcaption.cpp
                                      exif/exifdatetime.cpp
                                      exif/exifadjust.cpp
                                      exif/exiflens.cpp
                                      exif/exifdevice.cpp
                                      exif/exiflight.cpp
                                      iptc/iptceditwidget.cpp
                                      iptc/iptccontent.cpp
                                      iptc/iptcsubjects.cpp
                                      iptc/iptckeywords.cpp
                                      iptc/iptccategories.cpp
                                      iptc/iptccredits.cpp
                                      iptc/iptcproperties.cpp
                                      iptc/iptcstatus.cpp
                                      iptc/iptcorigin.cpp
                                      iptc/iptcenvelope.cpp
                                      xmp/xmpeditwidget.cpp
                                      xmp/xmpcontent.cpp
                                      xmp/xmpkeywords.cpp
                                      xmp/xmpcategories.cpp
                                      xmp/xmpsubjects.cpp
                                      xmp/xmporigin.cpp
                                      xmp/xmpcredits.cpp
                                      xmp/xmpstatus.cpp
                                      xmp/xmpproperties.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_metadataedit ${kipiplugin_metadataedit_PART_SRCS})

target_link_libraries(kipiplugin_metadataedit
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

configure_file(plugin/kipiplugin_metadataedit.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_metadataedit.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_metadataedit.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_metadataedit                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_metadataeditui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
