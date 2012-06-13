/*
 *  Copyright (C) 2011 Tuomo Penttinen, all rights reserved.
 *
 *  Author: Tuomo Penttinen <tp@herqq.org>
 *
 *  This file is part of Herqq UPnP Av (HUPnPAv) library.
 *
 *  Herqq UPnP Av is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Herqq UPnP Av is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Herqq UPnP Av. If not, see <http://www.gnu.org/licenses/>.
 */

#include "hconnectionmanager_sinkservice_p.h"
#include "hmediarenderer_device_p.h"

#include "../hav_global_p.h"
#include "../common/hprotocolinfo.h"

#include "../connectionmanager/hconnectioninfo.h"
#include "../connectionmanager/hprepareforconnection_result.h"

namespace Herqq
{

namespace Upnp
{

namespace Av
{

/*******************************************************************************
 * HConnectionManagerSinkService
 ******************************************************************************/
HConnectionManagerSinkService::HConnectionManagerSinkService() :
    HConnectionManagerService(), m_owner(0)
{
}

HConnectionManagerSinkService::~HConnectionManagerSinkService()
{
}

bool HConnectionManagerSinkService::finalizeInit(QString* errDescription)
{
    Q_UNUSED(errDescription)
    setSinkProtocolInfo(HProtocolInfo("http-get:*:*:*"));
    return true;
}

bool HConnectionManagerSinkService::init(HMediaRendererDevice* owner)
{
   Q_ASSERT(owner);
   m_owner = owner;
   return true;
}

qint32 HConnectionManagerSinkService::prepareForConnection(
    const HProtocolInfo& remoteProtocolInfo,
    const HConnectionManagerId& peerConnectionManager,
    qint32 peerConnectionId,
    HConnectionManagerInfo::Direction direction,
    HPrepareForConnectionResult* result)
{
    Q_ASSERT(result);

    if (!actions().value("PrepareForConnection"))
    {
        return UpnpOptionalActionNotImplemented;
    }

    if (direction != HConnectionManagerInfo::DirectionInput)
    {
        return HConnectionManagerInfo::IncompatibleDirections;
    }

    if (remoteProtocolInfo.protocol().compare("http-get", Qt::CaseInsensitive) &&
        remoteProtocolInfo.protocol() != "*")
    {
        return HConnectionManagerInfo::IncompatibleProtocolInfo;
    }

    if (!isMimetypeValid(
        remoteProtocolInfo.contentFormat(), sinkProtocolInfo()))
    {
        return HConnectionManagerInfo::IncompatibleProtocolInfo;
    }

    qint32 connectionId = nextId();
    qint32 avTransportId, rcsId;
    qint32 errCode = m_owner->prepareForConnection(
        remoteProtocolInfo.contentFormat(), connectionId, &avTransportId, &rcsId);

    if (errCode != UpnpSuccess)
    {
        return errCode;
    }

    HConnectionInfo connectionInfo(
        connectionId, avTransportId, rcsId,
        remoteProtocolInfo, peerConnectionManager, peerConnectionId,
        HConnectionManagerInfo::DirectionInput,
        HConnectionManagerInfo::StatusOk);

    result->setAvTransportId(avTransportId);
    result->setConnectionId(connectionId);
    result->setRcsId(rcsId);

    addConnection(connectionInfo);

    return UpnpSuccess;
}

qint32 HConnectionManagerSinkService::connectionComplete(qint32 connectionId)
{
    if (!actions().value("ConnectionComplete"))
    {
        return UpnpOptionalActionNotImplemented;
    }

    if (!connectionIds().contains(connectionId))
    {
        return HConnectionManagerInfo::InvalidConnectionReference;
    }

    m_owner->connectionComplete(connectionId);
    removeConnection(connectionId);

    return UpnpSuccess;
}

qint32 HConnectionManagerSinkService::nextId()
{
    return ++(lastConnectionId());
}

}
}
}
