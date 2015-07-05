#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(dlnaexport_xml_files
    dlnaexport_mediaserver_description.xml
    dlnaexport_connectionmanager_source_scpd.xml
    dlnaexport_contentdirectory_scpd.xml
   )

install(FILES       ${dlnaexport_xml_files}
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_dlnaexport/xml)
