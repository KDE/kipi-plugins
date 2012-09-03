/*
 *  Copyright (C) 2011 Tuomo Penttinen, all rights reserved.
 *
 *  Author: Tuomo Penttinen <tp@herqq.org>
 *
 *  This file is part of an application named HUpnpAvSimpleTestApp
 *  used for demonstrating how to use the Herqq UPnP A/V (HUPnPAv) library.
 *
 *  HUpnpAvSimpleTestApp is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  HUpnpAvSimpleTestApp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with HUpnpAvSimpleTestApp. If not, see <http://www.gnu.org/licenses/>.
 */

#include "mediarenderer_window.h"
#include "ui_mediarenderer_window.h"
#include "mediarenderer_connectionwindow.h"

#include <HUpnpCore/HUpnp>
#include <HUpnpCore/HDeviceInfo>
#include <HUpnpCore/HStateVariableEvent>
#include <HUpnpCore/HServerStateVariable>
#include <HUpnpCore/HDeviceHostConfiguration>

#include <HUpnpAv/HTransportState>
#include <HUpnpAv/HAvDeviceModelCreator>
#include <HUpnpAv/HAbstractMediaRendererDevice>
#include <HUpnpAv/HAbstractConnectionManagerService>
#include <HUpnpAv/HMediaRendererDeviceConfiguration>

#include <QtNetwork/QNetworkAccessManager>

// KDE includes

#include <kstandarddirs.h>
#include <kdebug.h>

using namespace Herqq::Upnp;
using namespace Herqq::Upnp::Av;

/*******************************************************************************
 * RendererConnectionManager
 *******************************************************************************/
HRendererConnection* RendererConnectionManager::doCreate(
    const QString& contentFormat, qint32 connectionId)
{
    MediaRendererConnectionWindow* mmWindow =
        new MediaRendererConnectionWindow(contentFormat, *m_owner->m_nam, 0);

    QString cf =
        contentFormat == "*" || contentFormat.isEmpty() ?
            "UNKNOWN" : contentFormat;

    if (!mmWindow->rendererConnectionManager())
    {
        return 0;
    }

    mmWindow->setWindowTitle(QString(
        "Media Renderer Connection #%1, content format: [%2]").arg(
            QString::number(connectionId), cf));

    mmWindow->show();

    bool ok = QObject::connect(m_owner, SIGNAL(destroyed()), mmWindow, SLOT(deleteLater()));
    Q_ASSERT(ok); Q_UNUSED(ok)

    return mmWindow->rendererConnectionManager();
}

RendererConnectionManager::RendererConnectionManager(MediaRendererWindow* owner) :
    HRendererConnectionManager(owner),
        m_owner(owner)
{
}

/*******************************************************************************
 * MediaRendererWindow
 *******************************************************************************/
MediaRendererWindow::MediaRendererWindow(QWidget* parent) :
    QMainWindow(parent),
        m_ui(new Ui::MediaRendererWindow), m_deviceHost(0),
        m_mm(new RendererConnectionManager(this)),
        m_nam(new QNetworkAccessManager(this))
{
    m_ui->setupUi(this);

    HMediaRendererDeviceConfiguration mediaRendererConfig;
    mediaRendererConfig.setRendererConnectionManager(m_mm, false);

    HAvDeviceModelCreator creator;
    creator.setMediaRendererConfiguration(mediaRendererConfig);

    QString filePath = KStandardDirs::locate("data", "kipiplugin_dlnaexport/xml/herqq_mediarenderer_description.xml");

    HDeviceConfiguration config;
    config.setPathToDeviceDescription(filePath);
    config.setCacheControlMaxAge(180);

    kDebug() << "filepath properly set : " << filePath;

    HDeviceHostConfiguration hostConfiguration;
    hostConfiguration.setDeviceModelCreator(creator);
    hostConfiguration.add(config);

    m_deviceHost = new HDeviceHost(this);
    if (!m_deviceHost->init(hostConfiguration))
    {
        qWarning() << m_deviceHost->errorDescription();
        Q_ASSERT(false);
    }

    m_mediaRenderer =
        qobject_cast<HAbstractMediaRendererDevice*>(
            m_deviceHost->rootDevices().at(0));

    HServerStateVariable* currentConnectionIDs =
        m_mediaRenderer->connectionManager()->stateVariables().value("CurrentConnectionIDs");

    bool ok = connect(
        currentConnectionIDs,
        SIGNAL(valueChanged(Herqq::Upnp::HServerStateVariable*,Herqq::Upnp::HStateVariableEvent)),
        this,
        SLOT(currentConnectionIDsChanged(Herqq::Upnp::HServerStateVariable*,Herqq::Upnp::HStateVariableEvent)));
    Q_ASSERT(ok); Q_UNUSED(ok)
}

MediaRendererWindow::~MediaRendererWindow()
{
    delete m_ui;
}

void MediaRendererWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MediaRendererWindow::currentConnectionIDsChanged(
    HServerStateVariable*, const HStateVariableEvent& event)
{
    QStringList connections = event.newValue().toString().split(",");

    m_ui->connectionsInfoTable->setRowCount(0);

    for(int i = 0; i < connections.size(); ++i)
    {
        QString cid = connections[i].trimmed();
        if (!cid.isEmpty())
        {
            m_ui->connectionsInfoTable->insertRow(i);

            QTableWidgetItem* connectionId = new QTableWidgetItem(cid);
            connectionId->setFlags(Qt::ItemIsEnabled);
            m_ui->connectionsInfoTable->setItem(i, 0, connectionId);

            HRendererConnection* connection = m_mm->connection(cid.toInt());
            Q_ASSERT(connection);

            const HRendererConnectionInfo* connectionInfo = connection->info();

            bool ok = connect(
                connectionInfo,
                SIGNAL(propertyChanged(Herqq::Upnp::Av::HRendererConnectionInfo*,Herqq::Upnp::Av::HRendererConnectionEventInfo)),
                this,
                SLOT(propertyChanged(Herqq::Upnp::Av::HRendererConnectionInfo*,Herqq::Upnp::Av::HRendererConnectionEventInfo)));
            Q_ASSERT(ok); Q_UNUSED(ok)

            QTableWidgetItem* status =
                new QTableWidgetItem(connectionInfo->transportState().toString());

            status->setFlags(Qt::ItemIsEnabled);

            m_ui->connectionsInfoTable->setItem(i, 1, status);
        }
    }
}

void MediaRendererWindow::propertyChanged(
    HRendererConnectionInfo* source, const HRendererConnectionEventInfo& eventInfo)
{
    if (eventInfo.propertyName() != "TransportState")
    {
        return;
    }

    for(int i = 0; i < m_ui->connectionsInfoTable->rowCount(); ++i)
    {
        QTableWidgetItem* cid = m_ui->connectionsInfoTable->item(i, 0);
        if (cid->data(Qt::DisplayRole).toInt() == source->connection()->connectionId())
        {
            QTableWidgetItem* transportStatus = m_ui->connectionsInfoTable->item(i, 1);
            transportStatus->setText(eventInfo.newValue());
        }
    }
}

void MediaRendererWindow::closeEvent(QCloseEvent*)
{
    emit closed();
}
