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

#include "hrendererconnection_manager.h"
#include "hrendererconnection_manager_p.h"

#include "hrendererconnection.h"

namespace Herqq
{

namespace Upnp
{

namespace Av
{

/*******************************************************************************
 * HRendererConnectionManagerPrivate
 ******************************************************************************/
HRendererConnectionManagerPrivate::HRendererConnectionManagerPrivate() :
    m_connections()
{
}

/*******************************************************************************
 * HRendererConnectionManager
 ******************************************************************************/
HRendererConnectionManager::HRendererConnectionManager(QObject* parent) :
    QObject(parent),
        h_ptr(new HRendererConnectionManagerPrivate())
{
}

HRendererConnectionManager::~HRendererConnectionManager()
{
    delete h_ptr;
}

void HRendererConnectionManager::destroyed_(QObject* obj)
{
    Connections::iterator it = h_ptr->m_connections.begin();
    for(; it != h_ptr->m_connections.end(); ++it)
    {
        if (it->second == obj)
        {
            qint32 cid = it->first;
            h_ptr->m_connections.erase(it);
            emit connectionRemoved(cid);
            break;
        }
    }
}

HRendererConnection* HRendererConnectionManager::create(
    const QString& contentFormat, qint32 connectionId)
{
    if (connection(connectionId))
    {
        return 0;
    }

    HRendererConnection* connection = doCreate(contentFormat, connectionId);
    if (!connection->parent())
    {
        connection->setParent(this);
    }

    bool ok = connect(connection, SIGNAL(destroyed(QObject*)), this, SLOT(destroyed_(QObject*)));
    Q_ASSERT(ok); Q_UNUSED(ok)

    connection->init(connectionId);
    h_ptr->m_connections.append(qMakePair(connectionId, connection));

    emit connectionAdded(connection->connectionId());

    return connection;
}

HRendererConnection* HRendererConnectionManager::connection(qint32 cid) const
{
    foreach(Connection connection, h_ptr->m_connections)
    {
        if (connection.first == cid)
        {
            return connection.second;
        }
    }
    return 0;
}

bool HRendererConnectionManager::connectionComplete(qint32 connectionId)
{
    Connections::iterator it = h_ptr->m_connections.begin();
    for(; it != h_ptr->m_connections.end(); ++it)
    {
        if (it->first == connectionId)
        {
            HRendererConnection* conn =  it->second;
            h_ptr->m_connections.erase(it);
            conn->dispose();
            emit connectionRemoved(connectionId);
            return true;
        }
    }
    return false;
}

}
}
}
