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

#ifndef HRENDERERCONNECTION_MANAGER_H_
#define HRENDERERCONNECTION_MANAGER_H_

#include <HUpnpAv/HUpnpAv>

#include <QtCore/QObject>

namespace Herqq
{

namespace Upnp
{

namespace Av
{

class HRendererConnectionManagerPrivate;

/*!
 * This class is used to create HRendererConnection instances.
 *
 * \headerfile hrendererconnection_manager.h HRendererConnectionManager
 *
 * \ingroup hupnp_av_mediarenderer
 *
 * \remarks This class is not thread-safe.
 *
 * \sa HRendererConnection
 */
class H_UPNP_AV_EXPORT HRendererConnectionManager :
    public QObject
{
Q_OBJECT

private Q_SLOTS:

    void destroyed_(QObject*);

protected:

    HRendererConnectionManagerPrivate* h_ptr;

    /*!
     * Creates a new instance.
     *
     * \param parent specifies the parent \c QObject, if any.
     */
    HRendererConnectionManager(QObject* parent = 0);

    /*!
     * Creates a new HRendererConnection instance.
     *
     * \param contentFormat specifies the MIME type(s) the connection is supposed
     * to handle. This should be either a valid MIME type or "*" when the
     * connection is supposed to handle any type of data.
     *
     * \param connectionId specifies the connection ID usually issued by a
     * ConnectionManager to which this instance is associated.
     *
     * \return a new HRendererConnection instance or a null pointer if no
     * instance could be created.
     *
     * \note When this function returns a valid HRendererConnection,
     * the base HRendererConnectionManager instance checks if \c QObject
     * parent has been set to the renderer connection and if not, assigns itself
     * as the parent of the new connection. However, you can assign the parent
     * yourself and you can delete the renderer connection at will.
     */
    virtual HRendererConnection* doCreate(
        const QString& contentFormat, qint32 connectionId) = 0;

public:

    /*!
     * Destroys the instance and every HRendererConnection instance it owns.
     *
     * \note You can change the \c QObject parent of a HRendererConnection instance
     * to modify this behavior.
     */
    virtual ~HRendererConnectionManager();

    /*!
     * Creates a new HRendererConnection instance.
     *
     * \param contentFormat specifies the MIME type(s) the connection is supposed
     * to handle. This should be either a valid MIME type or "*" when the
     * connection is supposed to handle any type of data.
     *
     * \param connectionId specifies the connection ID usually issued by a
     * ConnectionManager to which this instance is associated.
     *
     * \return a new HRendererConnection instance or a null pointer if no
     * instance could be created.
     */
    HRendererConnection* create(const QString& contentFormat, qint32 connectionId);

    /*!
     * Returns an HRendererConnection instance managed by this manager that
     * has the specified connectionId.
     *
     * \param cid specifies the connection ID.
     *
     * \return an HRendererConnection instance managed by this manager that
     * has the specified connectionId.
     */
    HRendererConnection* connection(qint32 cid) const;

    /*!
     * Instructs the instance to "dispose" a particular renderer connection and
     * remove it from the control of the manager.
     *
     * \param connectionId specifies the connection ID.
     *
     * \return \e true if a renderer connection matching the specified ID
     * was found and removed.
     *
     * \note This does not delete the renderer connection instance.
     *
     * \sa HRendererConnection::disposed()
     */
    bool connectionComplete(qint32 connectionId);

Q_SIGNALS:

    /*!
     * This signal is emitted when a new HRendererConnection has been added
     * into the control of this instance.
     *
     * \param connectionId specifies the connection ID of the new HRendererConnection
     * instance.
     *
     * \sa connectionRemoved()
     */
    void connectionAdded(qint32 connectionId);

    /*!
     * This signal is emitted when a HRendererConnection has been removed
     * from the control of this instance.
     *
     * \param connectionId specifies the connection ID of the HRendererConnection
     * instance that was removed.
     *
     * \sa connectionAdded()
     */
    void connectionRemoved(qint32 connectionId);

};

}
}
}

#endif /* HRENDERERCONNECTION_MANAGER_H_ */
