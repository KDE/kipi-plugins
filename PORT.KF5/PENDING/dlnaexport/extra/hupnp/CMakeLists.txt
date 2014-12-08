#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/src
                    src
                    ${QT_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_BINARY_DIR}/include
                    ${QTSOAP_INCLUDE_DIRS}
                   )

include(${QT_USE_FILE})

set(HUpnp_SRCS

    src/dataelements/hactioninfo.cpp
    src/dataelements/hdeviceinfo.cpp
    src/dataelements/hdiscoverytype.cpp
    src/dataelements/hproduct_tokens.cpp
    src/dataelements/hresourcetype.cpp
    src/dataelements/hserviceid.cpp
    src/dataelements/hserviceinfo.cpp
    src/dataelements/hstatevariableinfo.cpp
    src/dataelements/hudn.cpp
    src/dataelements/hvaluerange_p.cpp

    src/devicehosting/hddoc_parser_p.cpp
    src/devicehosting/hdevicestorage_p.cpp
    src/devicehosting/hmodelcreation_p.cpp

    src/devicehosting/controlpoint/hclientmodel_creator_p.cpp
    src/devicehosting/controlpoint/hcontrolpoint_configuration.cpp
    src/devicehosting/controlpoint/hcontrolpoint.cpp
    src/devicehosting/controlpoint/hcontrolpoint_dataretriever_p.cpp
    src/devicehosting/controlpoint/hdevicebuild_p.cpp
    src/devicehosting/controlpoint/hevent_subscriptionmanager_p.cpp
    src/devicehosting/controlpoint/hevent_subscription_p.cpp

    src/devicehosting/devicehost/hdevicehost_configuration.cpp
    src/devicehosting/devicehost/hdevicehost.cpp
    src/devicehosting/devicehost/hdevicehost_dataretriever_p.cpp
    src/devicehosting/devicehost/hdevicehost_http_server_p.cpp
    src/devicehosting/devicehost/hdevicehost_ssdp_handler_p.cpp
    src/devicehosting/devicehost/hevent_notifier_p.cpp
    src/devicehosting/devicehost/hevent_subscriber_p.cpp
    src/devicehosting/devicehost/hservermodel_creator_p.cpp

    src/devicehosting/messages/hcontrol_messages_p.cpp
    src/devicehosting/messages/hevent_messages_p.cpp
    src/devicehosting/messages/hnt_p.cpp
    src/devicehosting/messages/hsid_p.cpp
    src/devicehosting/messages/htimeout_p.cpp

    src/devicemodel/hactionarguments.cpp
    src/devicemodel/hactions_setupdata.cpp
    src/devicemodel/hasyncop.cpp
    src/devicemodel/hdevicemodel_infoprovider.cpp
    src/devicemodel/hdevicemodel_validator.cpp
    src/devicemodel/hdevices_setupdata.cpp
    src/devicemodel/hexecargs.cpp
    src/devicemodel/hservices_setupdata.cpp
    src/devicemodel/hstatevariable_event.cpp
    src/devicemodel/hstatevariables_setupdata.cpp

    src/devicemodel/client/hclientaction.cpp
    src/devicemodel/client/hclientactionop.cpp
    src/devicemodel/client/hclientadapterop.cpp
    src/devicemodel/client/hclientdevice_adapter.cpp
    src/devicemodel/client/hclientdevice.cpp
    src/devicemodel/client/hclientservice_adapter.cpp
    src/devicemodel/client/hclientservice.cpp
    src/devicemodel/client/hclientstatevariable.cpp

    src/devicemodel/server/hdevicemodelcreator.cpp
    src/devicemodel/server/hserveraction.cpp
    src/devicemodel/server/hserverdevice.cpp
    src/devicemodel/server/hserverservice.cpp
    src/devicemodel/server/hserverstatevariable.cpp

    src/general/hclonable.cpp
    src/general/hlogger_p.cpp
    src/general/hupnp_datatypes.cpp
    src/general/hupnp_global.cpp
    src/general/hupnpinfo.cpp

    src/http/hhttp_asynchandler_p.cpp
    src/http/hhttp_header_p.cpp
    src/http/hhttp_messagecreator_p.cpp
    src/http/hhttp_messaginginfo_p.cpp
    src/http/hhttp_server_p.cpp
    src/http/hhttp_utils_p.cpp

    src/socket/hendpoint.cpp
    src/socket/hmulticast_socket.cpp

    src/ssdp/hdiscovery_messages.cpp
    src/ssdp/hssdp.cpp
    src/ssdp/hssdp_messagecreator_p.cpp

    src/utils/hmisc_utils_p.cpp
    src/utils/hsysutils_p.cpp
    src/utils/hthreadpool_p.cpp
   )

set(HUpnp_MOC_HDRS

    src/devicehosting/devicehost/hdevicehost_p.h
    src/devicehosting/devicehost/hserverdevicecontroller_p.h
    src/devicehosting/devicehost/hevent_subscriber_p.h
    src/devicehosting/devicehost/hevent_notifier_p.h
    src/devicehosting/devicehost/hdevicehost_ssdp_handler_p.h
    src/devicehosting/devicehost/hdevicehost_http_server_p.h
    src/devicehosting/devicehost/hdevicehost.h
    src/devicehosting/controlpoint/hdevicebuild_p.h
    src/devicehosting/controlpoint/hcontrolpoint.h
    src/devicehosting/controlpoint/hcontrolpoint_dataretriever_p.h
    src/devicehosting/controlpoint/hcontrolpoint_p.h
    src/devicehosting/controlpoint/hevent_subscriptionmanager_p.h
    src/devicehosting/controlpoint/hevent_subscription_p.h

    src/devicemodel/server/hserverservice.h
    src/devicemodel/server/hserveraction.h
    src/devicemodel/server/hserverdevice.h
    src/devicemodel/server/hserverstatevariable.h
    src/devicemodel/client/hclientservice.h
    src/devicemodel/client/hclientservice_adapter.h
    src/devicemodel/client/hdefault_clientdevice_p.h
    src/devicemodel/client/hclientaction_p.h
    src/devicemodel/client/hclientstatevariable.h
    src/devicemodel/client/hclientaction.h
    src/devicemodel/client/hclientdevice.h
    src/devicemodel/client/hclientdevice_adapter.h

    src/http/hhttp_asynchandler_p.h
    src/http/hhttp_server_p.h

    src/ssdp/hssdp.h

    src/utils/hthreadpool_p.h
   )

QT4_WRAP_CPP(HUpnp_MOC_SRCS ${HUpnp_MOC_HDRS})

set(SRCS ${HUpnp_SRCS} ${HUpnp_MOC_SRCS})

foreach(_currentfile ${SRCS})
    if(NOT MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
endforeach()

add_library(HUpnp STATIC ${SRCS})

target_link_libraries(HUpnp ${QTSOAP_LIBRARIES} ${QT_LIBRARIES})

if(NOT MSVC)
    set_target_properties(HUpnp PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

#install(TARGETS HUpnp ARCHIVE DESTINATION lib ${INSTALL_TARGETS_DEFAULT_ARGS})
