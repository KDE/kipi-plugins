SET(upnp_xml_files herqq_mediaserver_description.xml herqq_connectionmanager_source_scpd.xml herqq_contentdirectory_scpd.xml)
INSTALL(FILES  ${upnp_xml_files}
       DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_dlnaexport/xml )
