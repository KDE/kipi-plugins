SET(dlnaexport_xml_files
    dlnaexport_mediaserver_description.xml
    dlnaexport_connectionmanager_source_scpd.xml
    dlnaexport_contentdirectory_scpd.xml
   )

INSTALL(FILES       ${dlnaexport_xml_files}
        DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_dlnaexport/xml)
