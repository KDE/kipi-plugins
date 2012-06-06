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

#include "mediaserver_window.moc"

#include <HUpnpCore/HUpnp>
#include <HUpnpCore/HDeviceInfo>
#include <HUpnpCore/HDeviceHostConfiguration>

#include <HUpnpAv/HRootDir>
#include <HUpnpAv/HAvDeviceModelCreator>
#include <HUpnpAv/HMediaServerDeviceConfiguration>

#include <HUpnpAv/HFileSystemDataSource>
#include <HUpnpAv/HContentDirectoryServiceConfiguration>


using namespace Herqq::Upnp;
using namespace Herqq::Upnp::Av;

/*******************************************************************************
 * MediaServerWindow
 *******************************************************************************/
MediaServerWindow::MediaServerWindow( QObject* parent )
    : QObject(parent),
      m_datasource( 0 )
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
    config.setPathToDeviceDescription("./descriptions/herqq_mediaserver_description.xml");
    config.setCacheControlMaxAge(180);

    HDeviceHostConfiguration hostConfiguration;
    hostConfiguration.setDeviceModelCreator(creator);
    hostConfiguration.add(config);

    // 6) Initialize the HDeviceHost.
    m_deviceHost = new HDeviceHost(this);
    if (!m_deviceHost->init(hostConfiguration))
    {
        Q_ASSERT_X(false, "", m_deviceHost->errorDescription().toLocal8Bit());
    }
}

MediaServerWindow::~MediaServerWindow()
{
     delete m_datasource;
}

void MediaServerWindow::on_addContentButton_clicked(QString dirName, bool mode)
{
    if (!dirName.isEmpty())
    {
        HRootDir::ScanMode smode = mode ?
                HRootDir::RecursiveScan : HRootDir::SingleDirectoryScan;

        HRootDir rd(dirName, smode);
        m_datasource->add(rd);
    }
}
