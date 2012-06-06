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

#ifndef MEDIASERVER_WINDOW_H
#define MEDIASERVER_WINDOW_H

#include <HUpnpAv/HUpnpAv>
#include <HUpnpCore/HDeviceHost>

class MediaServerWindow : public QObject
{
    Q_OBJECT

public:

    MediaServerWindow(QObject* parent = 0);
    virtual ~MediaServerWindow();
    void on_addContentButton_clicked(QString, bool);


private:

    Herqq::Upnp::HDeviceHost*               m_deviceHost;
    Herqq::Upnp::Av::HFileSystemDataSource* m_datasource;
};

#endif // MEDIASERVER_WINDOW_H
