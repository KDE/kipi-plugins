/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2011 by Tuomo Penttinen <tp at herqq dot org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "hupnpmediaserver.moc"

// KDE includes

#include <kstandarddirs.h>
#include <kdebug.h>
#include <kurl.h>

// libHUpnp includes

#include <HUpnpCore/HUpnp>
#include <HUpnpCore/HDeviceInfo>
#include <HUpnpCore/HDeviceHost>
#include <HUpnpCore/HDeviceHostConfiguration>
#include <HUpnpAv/HUpnpAv>
#include <HUpnpAv/HRootDir>
#include <HUpnpAv/HImageItem>
#include <HUpnpAv/HPhoto>
#include <HUpnpAv/HContainer>
#include <HUpnpAv/HAvDeviceModelCreator>
#include <HUpnpAv/HMediaServerDeviceConfiguration>
#include <HUpnpAv/HFileSystemDataSource>
#include <HUpnpAv/HContentDirectoryServiceConfiguration>

// Qt includes

#include <QList>
#include <QFile>

using namespace Herqq::Upnp;
using namespace Herqq::Upnp::Av;

namespace KIPIDLNAExportPlugin
{

class MediaServer::Private
{
public:

    Private()
    {
        deviceHost = 0;
        datasource = 0;
    }

    HDeviceHost*           deviceHost;
    HFileSystemDataSource* datasource;
};

MediaServer::MediaServer(QObject* const parent)
    : QObject(parent), d(new Private)
{
    // Configure a data source
    HFileSystemDataSourceConfiguration datasourceConfig;

    // Here you could configure the data source in more detail if needed. For example,
    // you could add "root directories" to the configuration and the data source
    // would scan those directories for media content upon initialization.
    d->datasource = new HFileSystemDataSource(datasourceConfig);

    // Configure ContentDirectoryService by providing it access to the desired data source.
    HContentDirectoryServiceConfiguration cdsConfig;
    cdsConfig.setDataSource(d->datasource, false);

    // Configure MediaServer by giving it the ContentDirectoryService configuration.
    HMediaServerDeviceConfiguration mediaServerConfig;
    mediaServerConfig.setContentDirectoryConfiguration(cdsConfig);

    // Setup the "Device Model Cretor" that HUPnP will use to create
    // appropriate UPnP A/V device and service instances. Here you provide the
    // MediaServer configuration HUPnP will pass to the MediaServer device instance.
    HAvDeviceModelCreator creator;
    creator.setMediaServerConfiguration(mediaServerConfig);

    // Setup the HDeviceHost with desired configuration info.
    HDeviceConfiguration config;

    QString filePath = KStandardDirs::locate("data", "kipiplugin_dlnaexport/xml/dlnaexport_mediaserver_description.xml");

    config.setPathToDeviceDescription(filePath);

    kDebug() << "filepath properly set : " << filePath;

    config.setCacheControlMaxAge(180);

    HDeviceHostConfiguration hostConfiguration;
    hostConfiguration.setDeviceModelCreator(creator);
    hostConfiguration.add(config);

    // Initialize the HDeviceHost.
    d->deviceHost = new HDeviceHost(this);

    if (!d->deviceHost->init(hostConfiguration))
    {
        kDebug() << "Initialization failed. Description : " << d->deviceHost->errorDescription().toLocal8Bit();
    }
}

MediaServer::~MediaServer()
{
     delete d->datasource;
     delete d;
}

void MediaServer::addImagesOnServer(const KUrl::List& imageUrlList)
{
    QList<HItem*> itemList;

    for (int i = 0; i<imageUrlList.size(); i++)
    {
        itemList.append(new HItem(QString(imageUrlList.at(i).fileName()), QString("0"), QString()));
        d->datasource->add(itemList.at(i), imageUrlList.at(i).path());
    }
}

void MediaServer::addImagesOnServer(const QMap<QString, KUrl::List>& collectionMap)
{
    QList<HContainer*> containerList;
    QList<HItem*>      itemList;
    QList<QString>     keys        = collectionMap.uniqueKeys();
    KUrl::List         imageUrlList;
    int                currentSize = 0;

    for (int i = 0; i<keys.size(); i++)
    {
        containerList.append(new HContainer(keys.at(i), QString("0")));
        d->datasource->add(containerList.at(i));

        imageUrlList.clear();
        imageUrlList = collectionMap.value(keys.at(i));
        currentSize  = itemList.size();

        for (int j = 0; j<imageUrlList.size(); j++)
        {
            itemList.append(new HItem(QString(imageUrlList.at(j).fileName()), containerList.at(i)->id(), QString()));
            d->datasource->add(itemList.at(j + currentSize), imageUrlList.at(j).path());
        }
    }
}

} // namespace KIPIDLNAExportPlugin
