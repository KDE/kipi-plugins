/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2011 Tuomo Penttinen <tp at herqq dot org>
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

#include "mediaserver_window.moc"

// KDE includes

#include <kstandarddirs.h>
#include <kdebug.h>

// libHUpnp includes

#include <HUpnpCore/HUpnp>
#include <HUpnpCore/HDeviceInfo>
#include <HUpnpCore/HDeviceHostConfiguration>
#include <HUpnpAv/HRootDir>
#include <HUpnpAv/HAvDeviceModelCreator>
#include <HUpnpAv/HMediaServerDeviceConfiguration>
#include <HUpnpAv/HFileSystemDataSource>
#include <HUpnpAv/HContentDirectoryServiceConfiguration>

namespace KIPIDLNAExportPlugin
{

MediaServerWindow::MediaServerWindow(QObject* const parent)
    : QObject(parent),
      m_datasource(0)
{
    // 1) Configure a data source
    HFileSystemDataSourceConfiguration datasourceConfig;
    // Here you could configure the data source in more detail if needed. For example,
    // you could add "root directories" to the configuration and the data source
    // would scan those directories for media content upon initialization.
    m_datasource = new HFileSystemDataSource(datasourceConfig);

    // 2) Configure ContentDirectoryService by providing it access to the desired data source.
    HContentDirectoryServiceConfiguration cdsConfig;
    cdsConfig.setDataSource(m_datasource, false);

    // 3) Configure MediaServer by giving it the ContentDirectoryService configuration.
    HMediaServerDeviceConfiguration mediaServerConfig;
    mediaServerConfig.setContentDirectoryConfiguration(cdsConfig);

    // 4) Setup the "Device Model Cretor" that HUPnP will use to create
    // appropriate UPnP A/V device and service instances. Here you provide the
    // MediaServer configuration HUPnP will pass to the MediaServer device instance.
    HAvDeviceModelCreator creator;
    creator.setMediaServerConfiguration(mediaServerConfig);

    // 5) Setup the HDeviceHost with desired configuration info.
    HDeviceConfiguration config;

    QString filePath = KStandardDirs::locate("data",
    "kipiplugin_dlnaexport/xml/herqq_mediaserver_description.xml");

    config.setPathToDeviceDescription(filePath);

    kDebug() << "filepath properly set : " << filePath;

    config.setCacheControlMaxAge(180);

    HDeviceHostConfiguration hostConfiguration;
    hostConfiguration.setDeviceModelCreator(creator);
    hostConfiguration.add(config);

    // 6) Initialize the HDeviceHost.
    m_deviceHost = new HDeviceHost(this);

    if (!m_deviceHost->init(hostConfiguration))
    {
    //    Q_ASSERT_X(false, "", m_deviceHost->errorDescription().toLocal8Bit());
        kDebug() << "Initialization failed. Description : " << m_deviceHost->errorDescription().toLocal8Bit();
    }
}

MediaServerWindow::~MediaServerWindow()
{
     delete m_datasource;
}

void MediaServerWindow::onAddContentButtonClicked(const QString& dirName, bool mode)
{
    if (!dirName.isEmpty())
    {
        HRootDir::ScanMode smode = mode ?
                HRootDir::RecursiveScan : HRootDir::SingleDirectoryScan;

        HRootDir rd(dirName, smode);
        m_datasource->add(rd);
    }
}

} // namespace KIPIDLNAExportPlugin
