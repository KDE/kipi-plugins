#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/src
                    src
                    ${QT_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../hupnp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../hupnp/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_BINARY_DIR}/include
                   )

include(${QT_USE_FILE})

set(HUpnpAv_SRCS

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

set(HUpnpAv_MOC_HDRS

    src/cds_model/cds_objects/htextitem.h
    src/cds_model/cds_objects/hstoragefolder.h
    src/cds_model/cds_objects/haudiochannelgroup.h
    src/cds_model/cds_objects/hvideoitem.h
    src/cds_model/cds_objects/haudiobook.h
    src/cds_model/cds_objects/hchannelgroup.h
    src/cds_model/cds_objects/hbookmarkitem.h
    src/cds_model/cds_objects/hvideobroadcast.h
    src/cds_model/cds_objects/hmovie.h
    src/cds_model/cds_objects/hphotoalbum.h
    src/cds_model/cds_objects/hmusicgenre.h
    src/cds_model/cds_objects/haudioitem.h
    src/cds_model/cds_objects/hvideochannelgroup.h
    src/cds_model/cds_objects/hmusictrack.h
    src/cds_model/cds_objects/hvideoprogram.h
    src/cds_model/cds_objects/hmusicvideoclip.h
    src/cds_model/cds_objects/hepgcontainer.h
    src/cds_model/cds_objects/hobject.h
    src/cds_model/cds_objects/halbum.h
    src/cds_model/cds_objects/hplaylistitem.h
    src/cds_model/cds_objects/hmusicalbum.h
    src/cds_model/cds_objects/hepgitem.h
    src/cds_model/cds_objects/hcontainer.h
    src/cds_model/cds_objects/hstoragesystem.h
    src/cds_model/cds_objects/hphoto.h
    src/cds_model/cds_objects/hplaylistcontainer.h
    src/cds_model/cds_objects/hperson.h
    src/cds_model/cds_objects/haudiobroadcast.h
    src/cds_model/cds_objects/hitem.h
    src/cds_model/cds_objects/himageitem.h
    src/cds_model/cds_objects/hgenrecontainer.h
    src/cds_model/cds_objects/hstoragevolume.h
    src/cds_model/cds_objects/haudioprogram.h
    src/cds_model/cds_objects/hbookmarkfolder.h
    src/cds_model/cds_objects/hmoviegenre.h
    src/cds_model/cds_objects/hmusicartist.h

    src/cds_model/datasource/hfsys_datasource.h
    src/cds_model/datasource/habstract_cds_datasource.h
    src/cds_model/datasource/hcds_datasource.h

    src/connectionmanager/habstractconnectionmanager_service.h
    src/connectionmanager/hconnectionmanager_adapter.h
    src/connectionmanager/hconnectionmanager_service_p.h

    src/contentdirectory/habstractcontentdirectory_service.h
    src/contentdirectory/hcontentdirectory_adapter.h
    src/contentdirectory/hcontentdirectory_service.h

    src/controlpoint/hav_controlpoint.h
    src/controlpoint/hav_controlpoint_p.h

    src/mediabrowser/hmediabrowser_p.h
    src/mediabrowser/hmediabrowser.h

    src/mediarenderer/habstractmediarenderer_device.h
    src/mediarenderer/hconnection.h
    src/mediarenderer/hconnectionmanager_sinkservice_p.h
    src/mediarenderer/hmediarenderer_adapter.h
    src/mediarenderer/hmediarenderer_device_p.h
    src/mediarenderer/hrendererconnection.h
    src/mediarenderer/hrendererconnection_info.h
    src/mediarenderer/hrendererconnection_manager.h
    src/mediarenderer/htransport_sinkservice_p.h

    src/mediaserver/habstractmediaserver_device.h
    src/mediaserver/hconnectionmanager_sourceservice_p.h
    src/mediaserver/hmediaserver_adapter.h
    src/mediaserver/hmediaserver_device_p.h

    src/renderingcontrol/habstractrenderingcontrol_service.h
    src/renderingcontrol/hrenderingcontrol_adapter.h
    src/renderingcontrol/hrenderingcontrol_service_p.h

    src/transport/habstract_avtransport_service.h
    src/transport/havtransport_adapter.h
   )

QT4_WRAP_CPP(HUpnpAv_MOC_SRCS ${HUpnpAv_MOC_HDRS})

set(SRCS ${HUpnpAv_SRCS} ${HUpnpAv_MOC_SRCS})

foreach(_currentfile ${SRCS})
    if(NOT MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
endforeach()

add_library(HUpnpAv STATIC ${SRCS})

target_link_libraries(HUpnpAv HUpnp ${QT_LIBRARIES})

if(NOT MSVC)
    set_target_properties(HUpnpAv PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

#install(TARGETS HUpnpAv ARCHIVE DESTINATION lib ${INSTALL_TARGETS_DEFAULT_ARGS})
