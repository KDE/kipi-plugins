REMOVE_DEFINITIONS(-DQT_USE_FAST_OPERATOR_PLUS)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/src
                    src
                    ${QT_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../hupnp/include/HUpnpCore
                    ${CMAKE_CURRENT_BINARY_DIR}/../hupnp/include/HUpnpCore
                    ${CMAKE_CURRENT_BINARY_DIR}/include
                   )

INCLUDE(${QT_USE_FILE})

SET(HUpnpAv_SRCS

    src/hav_devicemodel_creator.cpp
    src/hupnpavinfo.cpp
    src/hav_devicemodel_infoprovider.cpp
    src/hav_global.cpp

    src/renderingcontrol/habstractrenderingcontrol_service.cpp
    src/renderingcontrol/hrenderingcontrol_service_p.cpp
    src/renderingcontrol/hvolumedbrange_result.cpp
    src/renderingcontrol/hrenderingcontrol_adapter.cpp
    src/renderingcontrol/hchannel.cpp
    src/renderingcontrol/hrenderingcontrol_info.cpp

    src/transport/hduration.cpp
    src/transport/htransportinfo.cpp
    src/transport/htransportstate.cpp
    src/transport/habstract_avtransport_service.cpp
    src/transport/hplaymode.cpp
    src/transport/htransportaction.cpp
    src/transport/htransportsettings.cpp
    src/transport/hrecordqualitymode.cpp
    src/transport/hmediainfo.cpp
    src/transport/hpositioninfo.cpp
    src/transport/havtransport_adapter.cpp
    src/transport/hrecordmediumwritestatus.cpp
    src/transport/hseekinfo.cpp
    src/transport/hdevicecapabilities.cpp
    src/transport/havtransport_info.cpp

    src/cds_model/hgenre.cpp
    src/cds_model/hprice.cpp
    src/cds_model/hdeviceudn.cpp

    src/cds_model/cds_objects/hplaylistcontainer.cpp
    src/cds_model/cds_objects/hstoragevolume.cpp
    src/cds_model/cds_objects/halbum.cpp
    src/cds_model/cds_objects/hstoragesystem.cpp
    src/cds_model/cds_objects/hgenrecontainer.cpp
    src/cds_model/cds_objects/hmoviegenre.cpp
    src/cds_model/cds_objects/haudiobroadcast.cpp
    src/cds_model/cds_objects/hphoto.cpp
    src/cds_model/cds_objects/hbookmarkitem.cpp
    src/cds_model/cds_objects/hepgcontainer.cpp
    src/cds_model/cds_objects/hobject.cpp
    src/cds_model/cds_objects/hepgitem.cpp
    src/cds_model/cds_objects/hitem.cpp
    src/cds_model/cds_objects/hperson.cpp
    src/cds_model/cds_objects/hcontainer.cpp
    src/cds_model/cds_objects/hstoragefolder.cpp
    src/cds_model/cds_objects/hchannelgroup.cpp
    src/cds_model/cds_objects/haudioprogram.cpp
    src/cds_model/cds_objects/hmovie.cpp
    src/cds_model/cds_objects/hvideochannelgroup.cpp
    src/cds_model/cds_objects/haudioitem.cpp
    src/cds_model/cds_objects/hmusicartist.cpp
    src/cds_model/cds_objects/hmusicgenre.cpp
    src/cds_model/cds_objects/haudiochannelgroup.cpp
    src/cds_model/cds_objects/hphotoalbum.cpp
    src/cds_model/cds_objects/hmusicalbum.cpp
    src/cds_model/cds_objects/hbookmarkfolder.cpp
    src/cds_model/cds_objects/hmusicvideoclip.cpp
    src/cds_model/cds_objects/htextitem.cpp
    src/cds_model/cds_objects/haudiobook.cpp
    src/cds_model/cds_objects/himageitem.cpp
    src/cds_model/cds_objects/hplaylistitem.cpp
    src/cds_model/cds_objects/hvideobroadcast.cpp
    src/cds_model/cds_objects/hvideoprogram.cpp
    src/cds_model/cds_objects/hvideoitem.cpp
    src/cds_model/cds_objects/hmusictrack.cpp

    src/cds_model/hpersonwithrole.cpp
    src/cds_model/hdatetimerange.cpp
    src/cds_model/hmatching_id.cpp
    src/cds_model/hcontentduration.cpp
    src/cds_model/hforeignmetadata.cpp
    src/cds_model/hstatevariablecollection.cpp
    src/cds_model/hchannelgroupname.cpp
    src/cds_model/hsortinfo.cpp
    src/cds_model/hscheduledtime.cpp

    src/cds_model/datasource/habstract_cds_datasource.cpp
    src/cds_model/datasource/hrootdir.cpp
    src/cds_model/datasource/hcds_datasource.cpp
    src/cds_model/datasource/hfsys_datasource.cpp
    src/cds_model/datasource/hfsys_datasource_configuration.cpp
    src/cds_model/datasource/hcds_datasource_configuration.cpp

    src/cds_model/model_mgmt/hcdsproperties.cpp
    src/cds_model/model_mgmt/hcdsproperty_db.cpp
    src/cds_model/model_mgmt/hcdspropertyinfo.cpp
    src/cds_model/model_mgmt/hcds_fsys_reader_p.cpp
    src/cds_model/model_mgmt/hcdsproperty.cpp
    src/cds_model/model_mgmt/hcds_dlite_serializer.cpp
    src/cds_model/model_mgmt/hcdsobjectdata_p.cpp

    src/cds_model/hchannel_id.cpp
    src/cds_model/hcdsclassinfo.cpp
    src/cds_model/hprogramcode.cpp

    src/contentdirectory/hsearchresult.cpp
    src/contentdirectory/htransferprogressinfo.cpp
    src/contentdirectory/hcontentdirectory_info.cpp
    src/contentdirectory/habstractcontentdirectory_service.cpp
    src/contentdirectory/hcontentdirectory_adapter.cpp
    src/contentdirectory/hcontentdirectory_service.cpp
    src/contentdirectory/hcreateobjectresult.cpp
    src/contentdirectory/hcontentdirectory_serviceconfiguration.cpp
    src/contentdirectory/hfreeformqueryresult.cpp

    src/common/hprotocolinfo.cpp
    src/common/hradioband.cpp
    src/common/hrating.cpp
    src/common/hresource.cpp
    src/common/hstoragemedium.cpp

    src/mediaserver/hmediaserver_deviceconfiguration.cpp
    src/mediaserver/hmediaserver_adapter.cpp
    src/mediaserver/hmediaserver_info.cpp
    src/mediaserver/habstractmediaserver_device.cpp
    src/mediaserver/hconnectionmanager_sourceservice_p.cpp
    src/mediaserver/hmediaserver_device_p.cpp

    src/connectionmanager/hconnectionmanager_adapter.cpp
    src/connectionmanager/hprepareforconnection_result.cpp
    src/connectionmanager/hconnectionmanager_info.cpp
    src/connectionmanager/hprotocolinforesult.cpp
    src/connectionmanager/hconnectionmanager_id.cpp
    src/connectionmanager/habstractconnectionmanager_service.cpp
    src/connectionmanager/hconnectioninfo.cpp
    src/connectionmanager/hconnectionmanager_service_p.cpp

    src/mediabrowser/hmediabrowser.cpp
    src/mediabrowser/hcds_lastchange_info.cpp

    src/controlpoint/hav_controlpoint.cpp
    src/controlpoint/hav_controlpoint_configuration.cpp

    src/mediarenderer/hrendererconnection_manager.cpp
    src/mediarenderer/hrendererconnection.cpp
    src/mediarenderer/hmediarenderer_adapter.cpp
    src/mediarenderer/hconnection.cpp
    src/mediarenderer/habstractmediarenderer_device.cpp
    src/mediarenderer/hrcs_lastchange_info.cpp
    src/mediarenderer/hrendererconnection_info.cpp
    src/mediarenderer/htransport_sinkservice_p.cpp
    src/mediarenderer/hmediarenderer_info.cpp
    src/mediarenderer/havt_lastchange_info.cpp
    src/mediarenderer/hmediarenderer_deviceconfiguration.cpp
    src/mediarenderer/hmediarenderer_device_p.cpp
    src/mediarenderer/hconnectionmanager_sinkservice_p.cpp
)

SET(HUpnpAv_MOC_HDRS

    src/hav_defs.h
    src/hav_devicemodel_creator.h
    src/hav_devicemodel_infoprovider.h
    src/hupnpavinfo.h
    src/hav_fwd.h
    src/hav_global_p.h
    src/hav_global.h

    src/renderingcontrol/habstractrenderingcontrol_service_p.h
    src/renderingcontrol/hvolumedbrange_result.h
    src/renderingcontrol/hrenderingcontrol_adapter_p.h
    src/renderingcontrol/hrenderingcontrol_adapter.h
    src/renderingcontrol/hrenderingcontrol_service_p.h
    src/renderingcontrol/habstractrenderingcontrol_service.h
    src/renderingcontrol/hchannel.h
    src/renderingcontrol/hrenderingcontrol_info.h

    src/transport/hseekinfo.h
    src/transport/hdevicecapabilities.h
    src/transport/habstract_avtransport_service.h
    src/transport/habstract_avtransport_service_p.h
    src/transport/hrecordqualitymode.h
    src/transport/htransportinfo.h
    src/transport/havtransport_adapter.h
    src/transport/htransportsettings.h
    src/transport/hpositioninfo.h
    src/transport/havtransport_adapter_p.h
    src/transport/htransportstate.h
    src/transport/havtransport_info.h
    src/transport/hrecordmediumwritestatus.h
    src/transport/htransportaction.h
    src/transport/hduration.h
    src/transport/hplaymode.h
    src/transport/hmediainfo.h

    src/cds_model/hchannelgroupname.h
    src/cds_model/hgenre.h
    src/cds_model/hforeignmetadata.h
    src/cds_model/hmatching_id.h
    src/cds_model/hchannel_id.h
    src/cds_model/cds_objects/hvideoitem_p.h
    src/cds_model/cds_objects/hmusictrack_p.h
    src/cds_model/cds_objects/hplaylistcontainer_p.h
    src/cds_model/cds_objects/htextitem.h
    src/cds_model/cds_objects/hstoragevolume_p.h
    src/cds_model/cds_objects/hstoragefolder.h
    src/cds_model/cds_objects/halbum_p.h
    src/cds_model/cds_objects/haudiochannelgroup.h
    src/cds_model/cds_objects/hstoragesystem_p.h
    src/cds_model/cds_objects/hvideoitem.h
    src/cds_model/cds_objects/haudiobook.h
    src/cds_model/cds_objects/hchannelgroup.h
    src/cds_model/cds_objects/hbookmarkitem.h
    src/cds_model/cds_objects/hgenrecontainer_p.h
    src/cds_model/cds_objects/hmoviegenre_p.h
    src/cds_model/cds_objects/hvideobroadcast.h
    src/cds_model/cds_objects/hmovie.h
    src/cds_model/cds_objects/haudiobroadcast_p.h
    src/cds_model/cds_objects/hphoto_p.h
    src/cds_model/cds_objects/hphotoalbum.h
    src/cds_model/cds_objects/hmusicgenre.h
    src/cds_model/cds_objects/haudioitem.h
    src/cds_model/cds_objects/hbookmarkitem_p.h
    src/cds_model/cds_objects/hepgcontainer_p.h
    src/cds_model/cds_objects/hvideochannelgroup.h
    src/cds_model/cds_objects/hmusictrack.h
    src/cds_model/cds_objects/hobject_p.h
    src/cds_model/cds_objects/hvideoprogram.h
    src/cds_model/cds_objects/hmusicvideoclip.h
    src/cds_model/cds_objects/hepgitem_p.h
    src/cds_model/cds_objects/hitem_p.h
    src/cds_model/cds_objects/hepgcontainer.h
    src/cds_model/cds_objects/hobject.h
    src/cds_model/cds_objects/hperson_p.h
    src/cds_model/cds_objects/halbum.h
    src/cds_model/cds_objects/hplaylistitem.h
    src/cds_model/cds_objects/hcontainer_p.h
    src/cds_model/cds_objects/hstoragefolder_p.h
    src/cds_model/cds_objects/hchannelgroup_p.h
    src/cds_model/cds_objects/hmusicalbum.h
    src/cds_model/cds_objects/hepgitem.h
    src/cds_model/cds_objects/haudioprogram_p.h
    src/cds_model/cds_objects/hmovie_p.h
    src/cds_model/cds_objects/hcontainer.h
    src/cds_model/cds_objects/hvideochannelgroup_p.h
    src/cds_model/cds_objects/hstoragesystem.h
    src/cds_model/cds_objects/hphoto.h
    src/cds_model/cds_objects/hplaylistcontainer.h
    src/cds_model/cds_objects/hperson.h
    src/cds_model/cds_objects/haudiobroadcast.h
    src/cds_model/cds_objects/haudioitem_p.h
    src/cds_model/cds_objects/hmusicartist_p.h
    src/cds_model/cds_objects/hitem.h
    src/cds_model/cds_objects/himageitem.h
    src/cds_model/cds_objects/hgenrecontainer.h
    src/cds_model/cds_objects/hmusicgenre_p.h
    src/cds_model/cds_objects/haudiochannelgroup_p.h
    src/cds_model/cds_objects/hstoragevolume.h
    src/cds_model/cds_objects/haudioprogram.h
    src/cds_model/cds_objects/hphotoalbum_p.h
    src/cds_model/cds_objects/hbookmarkfolder.h
    src/cds_model/cds_objects/hmusicalbum_p.h
    src/cds_model/cds_objects/hbookmarkfolder_p.h
    src/cds_model/cds_objects/hmusicvideoclip_p.h
    src/cds_model/cds_objects/htextitem_p.h
    src/cds_model/cds_objects/hmoviegenre.h
    src/cds_model/cds_objects/haudiobook_p.h
    src/cds_model/cds_objects/hmusicartist.h
    src/cds_model/cds_objects/himageitem_p.h
    src/cds_model/cds_objects/hplaylistitem_p.h
    src/cds_model/cds_objects/hvideobroadcast_p.h
    src/cds_model/cds_objects/hvideoprogram_p.h
    src/cds_model/hpersonwithrole.h
    src/cds_model/hprogramcode.h
    src/cds_model/hstatevariablecollection.h
    src/cds_model/hprice.h
    src/cds_model/hscheduledtime.h
    src/cds_model/hcdsclassinfo.h
    src/cds_model/datasource/habstract_cds_datasource_p.h
    src/cds_model/datasource/hcds_datasource.h
    src/cds_model/datasource/hrootdir.h
    src/cds_model/datasource/hcds_datasource_p.h
    src/cds_model/datasource/hfsys_datasource_p.h
    src/cds_model/datasource/hfsys_datasource_configuration_p.h
    src/cds_model/datasource/hfsys_datasource_configuration.h
    src/cds_model/datasource/hfsys_datasource.h
    src/cds_model/datasource/habstract_cds_datasource.h
    src/cds_model/datasource/hcds_datasource_configuration_p.h
    src/cds_model/datasource/hcds_datasource_configuration.h
    src/cds_model/model_mgmt/hcds_fsys_reader_p.h
    src/cds_model/model_mgmt/hcdsobjectdata_p.h
    src/cds_model/model_mgmt/hcdsproperty_db_p.h
    src/cds_model/model_mgmt/hcdsproperty_db.h
    src/cds_model/model_mgmt/hcdsproperty.h
    src/cds_model/model_mgmt/hcdsproperties.h
    src/cds_model/model_mgmt/hcdspropertyinfo.h
    src/cds_model/model_mgmt/hcds_dlite_serializer_p.h
    src/cds_model/model_mgmt/hcds_dlite_serializer.h
    src/cds_model/hcontentduration.h
    src/cds_model/hsortinfo.h
    src/cds_model/hdatetimerange.h
    src/cds_model/hdeviceudn.h

    src/contentdirectory/hcontentdirectory_info.h
    src/contentdirectory/htransferprogressinfo.h
    src/contentdirectory/hsearchresult.h
    src/contentdirectory/habstractcontentdirectory_service_p.h
    src/contentdirectory/hcontentdirectory_adapter.h
    src/contentdirectory/habstractcontentdirectory_service.h
    src/contentdirectory/hfreeformqueryresult.h
    src/contentdirectory/hcontentdirectory_adapter_p.h
    src/contentdirectory/hcontentdirectory_serviceconfiguration.h
    src/contentdirectory/hcontentdirectory_service_p.h
    src/contentdirectory/hcontentdirectory_serviceconfiguration_p.h
    src/contentdirectory/hcontentdirectory_service.h
    src/contentdirectory/hcreateobjectresult.h

    src/common/hrating.h
    src/common/hprotocolinfo.h
    src/common/hradioband.h
    src/common/hstoragemedium.h
    src/common/hresource.h

    src/mediaserver/hmediaserver_deviceconfiguration_p.h
    src/mediaserver/habstractmediaserver_device.h
    src/mediaserver/hmediaserver_device_p.h
    src/mediaserver/hmediaserver_deviceconfiguration.h
    src/mediaserver/hmediaserver_adapter_p.h
    src/mediaserver/hconnectionmanager_sourceservice_p.h
    src/mediaserver/hmediaserver_info.h
    src/mediaserver/hmediaserver_adapter.h

    src/connectionmanager/hconnectionmanager_adapter_p.h
    src/connectionmanager/hconnectionmanager_service_p.h
    src/connectionmanager/habstractconnectionmanager_service.h
    src/connectionmanager/hprepareforconnection_result.h
    src/connectionmanager/hprotocolinforesult.h
    src/connectionmanager/hconnectionmanager_id.h
    src/connectionmanager/hconnectionmanager_info.h
    src/connectionmanager/habstractconnectionmanager_service_p.h
    src/connectionmanager/hconnectioninfo.h
    src/connectionmanager/hconnectionmanager_adapter.h

    src/mediabrowser/hmediabrowser_p.h
    src/mediabrowser/hmediabrowser.h
    src/mediabrowser/hcds_lastchange_info.h

    src/controlpoint/hav_controlpoint.h
    src/controlpoint/hav_controlpoint_configuration.h
    src/controlpoint/hav_controlpoint_p.h
    src/controlpoint/hav_controlpoint_configuration_p.h

    src/mediarenderer/hmediarenderer_deviceconfiguration.h
    src/mediarenderer/hrendererconnection_manager_p.h
    src/mediarenderer/hmediarenderer_adapter.h
    src/mediarenderer/hrendererconnection_p.h
    src/mediarenderer/hmediarenderer_adapter_p.h
    src/mediarenderer/hconnection.h
    src/mediarenderer/hconnectionmanager_sinkservice_p.h
    src/mediarenderer/hrcs_lastchange_info.h
    src/mediarenderer/hmediarenderer_device_p.h
    src/mediarenderer/havt_lastchange_info.h
    src/mediarenderer/hrendererconnection_info_p.h
    src/mediarenderer/hrendererconnection.h
    src/mediarenderer/hmediarenderer_info.h
    src/mediarenderer/habstractmediarenderer_device.h
    src/mediarenderer/hmediarenderer_deviceconfiguration_p.h
    src/mediarenderer/htransport_sinkservice_p.h
    src/mediarenderer/hrendererconnection_manager.h
    src/mediarenderer/hrendererconnection_info.h
   )

QT4_WRAP_CPP(HUpnpAv_MOC_SRCS ${HUpnpAv_MOC_HDRS})

ADD_LIBRARY(HUpnpAv STATIC ${HUpnpAv_SRCS} ${HUpnpAv_MOC_SRCS})

TARGET_LINK_LIBRARIES(HUpnpAv HUpnp ${QT_LIBRARIES})

INSTALL(TARGETS HUpnpAv ARCHIVE DESTINATION lib ${INSTALL_TARGETS_DEFAULT_ARGS})
